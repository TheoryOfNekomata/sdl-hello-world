#if _WINDOWS
#include <dirent/dirent.h>
#else
#include <dirent.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <lzma/7z.h>
#include <lzma/7zFile.h>
#include <lzma/7zAlloc.h>
#include <lzma/7zCrc.h>
#include <lzma/7zBuf.h>

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

const char* assets_dir = "assets";
