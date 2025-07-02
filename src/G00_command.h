#ifndef G00_COMMAND_H
#define G00_COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "G00_config.h"

G00_ConfigCommand G00_CommandHelp;

G00_ConfigCommand G00_CommandMemoryPoolSizeBytes;
G00_ConfigCommand G00_CommandMemoryPoolMaxEntries;

G00_ConfigCommand G00_CommandVideoScreenSize;
G00_ConfigCommand G00_CommandVideoIdealFps;
G00_ConfigCommand G00_CommandVideoMaxLoadedTextures;
G00_ConfigCommand G00_CommandVideoMaxLoadedFonts;
G00_ConfigCommand G00_CommandVideoMaxLoadedSprites;

int G00_ConfigParseArgs(char[255], struct G00_ConfigCommandArguments, unsigned int* out0, ...);

int G00_AppApplyConfig(struct G00_App* app);

static struct G00_ConfigCommandMappingEntry G00_CONFIG_COMMAND_MAPPING[] = {
	{
		.name = "help",
		.execute_fn = G00_CommandHelp,
		.args = {
			.minimum = 0,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "command",
					.type = G00_CONFIG_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "video_screen_size",
		.execute_fn = G00_CommandVideoScreenSize,
		.args = {
			.minimum = 2,
			.maximum = 2,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "width",
					.type = G00_CONFIG_ARGUMENT_TYPE_U32,
				},
				{
					.name = "height",
					.type = G00_CONFIG_ARGUMENT_TYPE_U32,
				},
			},
		}
	},
	{
		.name = "video_ideal_fps",
		.execute_fn = G00_CommandVideoIdealFps,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "fps",
					.type = G00_CONFIG_ARGUMENT_TYPE_F32,
				},
			},
		}
	},
	{
		.name = "memory_pool_size_bytes",
		.execute_fn = G00_CommandMemoryPoolSizeBytes,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "bytes",
					.type = G00_CONFIG_ARGUMENT_TYPE_U64,
				},
			},
		},
	},
	{
		.name = "memory_pool_max_entries",
		.execute_fn = G00_CommandMemoryPoolMaxEntries,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "bytes",
					.type = G00_CONFIG_ARGUMENT_TYPE_U32,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_textures",
		.execute_fn = G00_CommandVideoMaxLoadedTextures,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "bytes",
					.type = G00_CONFIG_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_fonts",
		.execute_fn = G00_CommandVideoMaxLoadedFonts,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "bytes",
					.type = G00_CONFIG_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_sprites",
		.execute_fn = G00_CommandVideoMaxLoadedSprites,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_ConfigArgument[]) {
				{
					.name = "bytes",
					.type = G00_CONFIG_ARGUMENT_TYPE_U8,
				},
			},
		},
	},

	{
		.name = "msg_key",
		.execute_fn = NULL // TODO
	},
{
		.name = "menu",
		.execute_fn = NULL // TODO
	},
{
		.name = "item",
		.execute_fn = NULL // TODO
	},
{
		.name = "title",
		.execute_fn = NULL // TODO
	},
{
		.name = "description_text",
		.execute_fn = NULL // TODO
	},
{
		.name = "label",
		.execute_fn = NULL // TODO
	},
{
		.name = "menu_push",
		.execute_fn = NULL // TODO
	},
{
		.name = "menu_pop",
		.execute_fn = NULL // TODO
	},
{
		.name = "menu_show_start",
		.execute_fn = NULL // TODO
	},

	{
		.name = NULL,
		.execute_fn = NULL,
	},
};

#endif
