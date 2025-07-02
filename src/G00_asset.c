#include "G00_asset.h"

static ISzAlloc alloc_imp = {
	SzAlloc,
	SzFree,
};

static int Buf_EnsureSize(CBuf* dest, size_t size) {
	if (dest->size >= size)
		return 1;

	Buf_Free(dest, &alloc_imp);
	return Buf_Create(dest, size, &alloc_imp);
}

static SRes Utf16_To_Char(CBuf* buf, const UInt16* s
#ifndef MY_USE_UTF8
                          , UINT codePage
#endif
) {
	size_t len = 0;
	for (len = 0; s[len] != 0; len++) {
	} {
		const size_t size = len * 3 + 100;
		if (!Buf_EnsureSize(buf, size))
			return SZ_ERROR_MEM; {
			buf->data[0] = 0;
			if (len != 0) {
				const char defaultChar = '_';
				BOOL defUsed;
				const unsigned numChars = (unsigned) WideCharToMultiByte(
					codePage, 0, (LPCWSTR) s, (int) len, (char*) buf->data, (int) size, &defaultChar, &defUsed);
				if (numChars == 0 || numChars >= size)
					return SZ_ERROR_FAIL;
				buf->data[numChars] = 0;
			}
			return SZ_OK;
		}
	}
}

int G00_AssetLoad(const char* path, struct G00_MemoryState* out0_asset_stream) {
	if (out0_asset_stream == NULL) {
		return -1;
	}

	CFileInStream archive_stream;
	WRes res = InFile_Open(&archive_stream.file, path);
	if (res != 0) {
		fprintf(stderr, "Cannot open asset.\n");
		return -2;
	}

	FileInStream_CreateVTable(&archive_stream);
	archive_stream.wres = 0;

	CLookToRead2 look_stream;
	LookToRead2_CreateVTable(&look_stream, false);

	ISzAlloc alloc_imp = {
		SzAlloc,
		SzFree,
	};
	size_t buf_size = 1 << 18;
	look_stream.buf = (Byte*) ISzAlloc_Alloc(&alloc_imp, buf_size);
	if (!look_stream.buf) {
		return -2;
	}
	look_stream.bufSize = buf_size;
	look_stream.realStream = &archive_stream.vt;
	LookToRead2_INIT(&look_stream);

	CrcGenerateTable();

	CSzArEx db;
	SzArEx_Init(&db);
	if (SzArEx_Open(&db, &look_stream.vt, &alloc_imp, &alloc_imp) != SZ_OK) {
		return -2;
	}

	unsigned int block_index = 0xFFFFFFFFu;
	Byte* out_buffer = 0;
	size_t out_buffer_size = 0;

	unsigned short* file_name = NULL;
	size_t file_name_len = 0;

	size_t processed_bytes_in_this_asset = 0;
	for (unsigned int i = 0; i < db.NumFiles; i += 1) {
		size_t offset = 0;
		size_t asset_data_processed_bytes = 0;
		if (SzArEx_IsDir(&db, i)) {
			continue;
		}
		size_t len = SzArEx_GetFileNameUtf16(&db, i, NULL);
		size_t file_size = SzArEx_GetFileSize(&db, i);
		if (len > file_name_len) {
			SzFree(NULL, file_name);
			file_name_len = len;
			file_name = (unsigned short*) SzAlloc(NULL, file_name_len * sizeof(file_name[0]));
			if (!file_name) {
				return -2;
			}
		}

		SzArEx_GetFileNameUtf16(&db, i, file_name);

		CBuf buf;
		Buf_Init(&buf);
		if (Utf16_To_Char(&buf, file_name, CP_OEMCP) == SZ_OK) {
			printf("%s %llu\n", buf.data, file_size);
		}

		// TODO how to first read metadata of archive

		if (SzArEx_Extract(&db, &look_stream.vt, i, &block_index, &out_buffer, &out_buffer_size, &offset,
												 &asset_data_processed_bytes, &alloc_imp, &alloc_imp) != SZ_OK) {
			ISzAlloc_Free(&alloc_imp, out_buffer);
			SzFree(NULL, file_name);
			SzArEx_Free(&db, &alloc_imp);
			File_Close(&archive_stream.file);
			return -3;
		}

		size_t out_offset = offset + out0_asset_stream->asset_data_processed_bytes;
		processed_bytes_in_this_asset += asset_data_processed_bytes;
		memcpy(out0_asset_stream->data + out_offset, out_buffer + offset, asset_data_processed_bytes);
		for (unsigned int i = 0; i < out0_asset_stream->config.pool_max_entries; i += 1) {
			if (out0_asset_stream->entries[i].name != NULL) {
				continue;
			}

			out0_asset_stream->entries[i].name = malloc(buf.size);
			memcpy(out0_asset_stream->entries[i].name, buf.data, buf.size);
			out0_asset_stream->entries[i].len = asset_data_processed_bytes;
			out0_asset_stream->entries[i].offset = out_offset;
			size_t path_len = strlen(path) + 1;
			out0_asset_stream->entries[i].reference_asset_path = malloc(path_len);
			memcpy(out0_asset_stream->entries[i].reference_asset_path, path, path_len);
			break;
		}

		Buf_Free(&buf, &alloc_imp);
	}

	out0_asset_stream->asset_data_processed_bytes += processed_bytes_in_this_asset;
	ISzAlloc_Free(&alloc_imp, out_buffer);
	SzFree(NULL, file_name);
	SzArEx_Free(&db, &alloc_imp);
	File_Close(&archive_stream.file);
	return 0;
}

const char* load_order_filename = "load.cfg";

int G00_AssetGenerateLoadOrder(const char* assets_dir_path) {
	DIR* dir = opendir(assets_dir_path);
	if (dir == NULL) {
		fprintf(stdout, "No assets found.\n");
		return 1;
	}

	size_t path_len = strlen(assets_dir_path);
	size_t load_order_filename_len = strlen(load_order_filename);
	char* load_order_path = malloc(path_len + 1 + load_order_filename_len + 1);
	sprintf(load_order_path, "%s/%s", assets_dir_path, load_order_filename);

	struct stat s;
	stat(load_order_path, &s);
	if (s.st_size > 0) {
		return 2;
	}

	FILE* fp = fopen(load_order_path, "wt");
	free(load_order_path);
	if (fp == NULL) {
		return -1;
	}

	const char* asset_filename_suffix = ".asset.7z";
	size_t asset_filename_suffix_len = strlen(asset_filename_suffix);
	dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".")) {
			continue;
		}
		if (!strcmp(entry->d_name, "..")) {
			continue;
		}
		bool is_filtered = true;
		for (unsigned int i = 0; i < asset_filename_suffix_len && is_filtered; i += 1) {
			is_filtered = asset_filename_suffix[i] == entry->d_name[entry->d_namlen - asset_filename_suffix_len + i];
		}
		if (!is_filtered) {
			continue;
		}

		fprintf(fp, "asset_load %s\n", entry->d_name);
	}

	fclose(fp);
	return 0;
}

int G00_AssetLoadFromOrder(const char* assets_dir_path, struct G00_MemoryState* out0_memory) {
	if (assets_dir_path == NULL) {
		return -1;
	}

	if (out0_memory == NULL) {
		return -1;
	}

	size_t path_len = strlen(assets_dir_path);
	size_t load_order_filename_len = strlen(load_order_filename);
	char* load_order_path = malloc(path_len + 1 + load_order_filename_len + 1);
	sprintf(load_order_path, "%s/%s", assets_dir_path, load_order_filename);

	FILE* fp = fopen(load_order_path, "rt");
	free(load_order_path);
	if (fp == NULL) {
		fprintf(stderr, "Cannot open asset loader. Doing nothing.\n");
		return 1;
	}

	char command[255];
	char arg[255];

	while (!feof(fp)) {
		fscanf(fp, "%s %s\n", &command, &arg);

		if (!strcmp(command, "load")) {
			char* asset_path = malloc(path_len + 1 + strlen(arg) + 1);

			sprintf(asset_path, "%s/%s", assets_dir_path, arg);
			fprintf(stdout, "Loading %s...\n", asset_path);

			int asset_load_response = G00_AssetLoad(asset_path, out0_memory);
			if (asset_load_response < 0) { // TODO fix default asset filenames
				fprintf(stderr, "Unrecoverable error.\n");
				return -2;
			}
			if (asset_load_response > 0) {
				fprintf(stdout, "Loaded with warnings.\n");
				continue;
			}
			fprintf(stdout, "Loaded successfully.\n");
		}
	}

	fclose(fp);
	return 0;
}
