#ifndef G00_COMMANDS_H
#define G00_COMMANDS_H

#include "G00_command.h"

G00_Command G00_XCommandHelp;
G00_Command G00_XCommandExit;

G00_CommandMemory G00_XCommandMemoryPoolSizeBytes;
G00_CommandMemory G00_XCommandMemoryPoolMaxEntries;
G00_CommandMemory G00_XCommandMemoryAssetLoad;

G00_CommandVideo G00_XCommandVideoScreenSize;
G00_CommandVideo G00_XCommandVideoIdealFps;
G00_CommandVideo G00_XCommandVideoMaxLoadedTextures;
G00_CommandVideo G00_XCommandVideoMaxLoadedFonts;
G00_CommandVideo G00_XCommandVideoMaxLoadedSprites;

G00_CommandUI G00_XCommandUIMsgKey;
G00_CommandUI G00_XCommandUIMenu;
G00_CommandUI G00_XCommandUIItem;
G00_CommandUI G00_XCommandUITitle;
G00_CommandUI G00_XCommandUIDescriptionText;
G00_CommandUI G00_XCommandUILabel;
G00_CommandUI G00_XCommandUIMenuPush;
G00_CommandUI G00_XCommandUIMenuPop;
G00_CommandUI G00_XCommandUIMenuShowStart;

static struct G00_CommandEntry G00_COMMAND_ENTRIES[] = {
	{
		.name = "help",
		.execute_fn = G00_XCommandHelp,
		.args = {
			.minimum = 0,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "command",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "video_screen_size",
		.execute_fn = G00_XCommandVideoScreenSize,
		.args = {
			.minimum = 2,
			.maximum = 2,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "width",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
				{
					.name = "height",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
			},
		}
	},
	{
		.name = "video_ideal_fps",
		.execute_fn = G00_XCommandVideoIdealFps,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "fps",
					.type = G00_COMMAND_ARGUMENT_TYPE_F32,
				},
			},
		}
	},
	{
		.name = "memory_pool_size_bytes",
		.execute_fn = G00_XCommandMemoryPoolSizeBytes,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U64,
				},
			},
		},
	},
	{
		.name = "memory_pool_max_entries",
		.execute_fn = G00_XCommandMemoryPoolMaxEntries,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_textures",
		.execute_fn = G00_XCommandVideoMaxLoadedTextures,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_fonts",
		.execute_fn = G00_XCommandVideoMaxLoadedFonts,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_sprites",
		.execute_fn = G00_XCommandVideoMaxLoadedSprites,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "memory_asset_load",
		.execute_fn = G00_XCommandMemoryAssetLoad,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "path",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			}
		}
	},

	{
		.name = "ui_msg_key",
		.execute_fn = G00_XCommandUIMsgKey,
		.args = {
			.minimum = 2,
			.maximum = 2,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "key",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
				{
					.name = "fallback_value",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			}
		}
	},
	{
		.name = "ui_menu",
		.execute_fn = G00_XCommandUIMenu,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "label",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "ui_item",
		.execute_fn = G00_XCommandUIItem,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "command",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "ui_title",
		.execute_fn = G00_XCommandUITitle,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "msg_key",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "ui_description_text",
		.execute_fn = G00_XCommandUIDescriptionText,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "msg_key",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "ui_label",
		.execute_fn = G00_XCommandUILabel,
		.args = {
			.minimum = 0,
			.maximum = 0,
			.defs = NULL,
		},
	},
	{
		.name = "ui_menu_push",
		.execute_fn = G00_XCommandUIMenuPush,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]){
				{
					.name = "label",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "ui_menu_pop",
		.execute_fn = G00_XCommandUIMenuPop,
		.args = {
			.minimum = 0,
			.maximum = 0,
			.defs = NULL,
		},
	},
	{
		.name = "ui_menu_show_start",
		.execute_fn = G00_XCommandUIMenuShowStart,
		.args = {
			.minimum = 0,
			.maximum = 0,
			.defs = NULL,
		},
	},
	{
		.name = "exit",
		.execute_fn = G00_XCommandExit,
		.args = {
			.minimum = 0,
			.maximum = 0,
			.defs = NULL,
		},
	},
	{
		.name = NULL,
		.execute_fn = NULL,
	},
};


#endif
