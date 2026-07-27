#include <stdio.h>

#include "G00_commands.h"
#include "G00_config.h"

enum G00_AppInitResult : int {
	G00_APP_INIT_RESULT_OK = 0,
	G00_APP_INIT_RESULT_MEMORY_ERROR = -1,
	G00_APP_INIT_RESULT_DATABASE_ERROR = -2,
	G00_APP_INIT_RESULT_FRAMEWORK_ERROR = -3,
	G00_APP_INIT_RESULT_FONT_RENDERER_ERROR = -4,
	G00_APP_INIT_RESULT_VIDEO_ERROR = -5,
};

enum G00_AppInitResult G00_AppInit(struct G00_App* app, int argc, char* argv[]) {
	bool load_custom_configs = true;
	for (unsigned int i = 1; i < argc; i += 1) {
		if (!strcmp("--ignore-custom", argv[i])) {
			load_custom_configs = false;
		}
	}

	G00_ConfigExecuteScript("default.app.cfg", app);
	if (load_custom_configs) {
		G00_ConfigExecuteScript("autoexec.app.cfg", app);
	}

	int memory_result = G00_MemoryInit(&app->memory);
	if (memory_result < 0) {
		return G00_APP_INIT_RESULT_MEMORY_ERROR;
	}

	int sqlite_result = G00_DatabaseInit(&app->db);
	if (sqlite_result < 0) {
		return G00_APP_INIT_RESULT_DATABASE_ERROR;
	}

	G00_ConfigExecuteScript("default.asset.cfg", app);
	if (load_custom_configs) {
		G00_ConfigExecuteScript("autoexec.asset.cfg", app);
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return G00_APP_INIT_RESULT_FRAMEWORK_ERROR;
	}

	if (!TTF_Init()) {
		fprintf(stderr, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());
		return G00_APP_INIT_RESULT_FONT_RENDERER_ERROR;
	}

	app->ticks = 0;
	app->video.app = app;
	if (G00_VideoInit(&app->video)) {
		return G00_APP_INIT_RESULT_VIDEO_ERROR;
	}

	G00_UIInit(&app->ui);
	G00_ConfigExecuteScript("default.menu.cfg", app);
	app->mode = G00_APP_MODE_UI;
	app->ui.current_menu = app->ui.menus->data;
	app->ui.current_item = app->ui.current_menu->children->data;

	app->game_state.menu_bg_asset_index = 0;
	app->game_state.menu_fg_asset_index = 0;
	app->game_state.primary_font_asset_index = 0;
	app->game_state.primary_font_index = 0;
	app->game_state.menu_bg_sprite_index = 0;
	app->game_state.menu_fg_sprite_index = 0;
	app->game_state.menu_fg_shadow0_sprite_index = 0;
	app->game_state.menu_fg_shadow1_sprite_index = 0;
	app->game_state.menu_fg_shadow2_sprite_index = 0;
	app->game_state.menu_fg_shadow3_sprite_index = 0;

	return G00_APP_INIT_RESULT_OK;
}

int G00_AppSetupMenu(struct G00_App* app) {
	if (app->ui.history_stack_index == 0) {
		// no menu + assets to load, stack is empty
		return 0;
	}

	struct G00_UIMenuParentNode* displayed_menu = app->ui.history_stack[app->ui.history_stack_index - 1];

	// This displays the label of the current menu
	//
	// const float half_screen_x = app->video.config.screen_width / 2.f;
	// unsigned int text_sprite_index;
	// int text_sprite_load_result = G00_VideoGenerateTextSprite(&app->video, font_index, displayed_menu->label, 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}, &text_sprite_index);
	// if (text_sprite_load_result < 0) {
	// 	fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
	// 	return -1;
	// } if (text_sprite_load_result > 0) {
	// 	fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	// }
	//
	// app->video.loaded_sprites[text_sprite_index].rect = (SDL_FRect) {
	// 	.x = half_screen_x - (app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->w / 2.f),
	// 	.y = app->video.config.screen_height - app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->h,
	// 	.w = app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->w,
	// 	.h = app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->h,
	// };

	struct G00_ListNode* child = displayed_menu->children;
	unsigned char menu_y = 0;
	int text_sprite_load_result;

	while (child != NULL) {
		union G00_UIMenuChildNode* node_data = child->data;
		struct G00_ListNode* message_entry = app->ui.messages;
		struct G00_MessageEntry* node_message = NULL;
		switch (node_data->base.type) {
			case G00_UI_MENU_CHILD_NODE_TYPE_ITEM:
				while (message_entry != NULL) {
					if (
						message_entry->data != NULL &&
						!strcmp(((struct G00_MessageEntry*) message_entry->data)->key, node_data->item.title)
					) {
						node_message = message_entry->data;
						break;
					}
					message_entry = message_entry->next;
				}

				text_sprite_load_result = G00_VideoGenerateTextSprite(
					&app->video,
					app->game_state.primary_font_index,
					node_message != NULL
						? node_message->fallback_value
						: node_data->item.title,
					13,
					&node_data->item.sprite_index
				);
				if (text_sprite_load_result < 0) {
					fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
					return -1;
				} if (text_sprite_load_result > 0) {
					fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
				}

				app->video.loaded_sprites[node_data->item.sprite_index].text.processing_nodes = app->ui.current_item == node_data ? &(struct G00_ListNode) {
					.data = &(struct G00_VideoFillColorProcessingNode) {
						.type = G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
						.color = (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0x00, .a = 0xFF },
					},
					.next = NULL,
				} : NULL;

				app->video.loaded_sprites[node_data->item.sprite_index].text.rect = (SDL_FRect) {
					.x = 100,
					//.y = app->video.config.screen_height - ,
					.y = 300 + menu_y * (app->video.loaded_surfaces[app->video.loaded_sprites[node_data->item.sprite_index].text.processed_surface_index]->h * 1.5f),
					.w = app->video.loaded_surfaces[app->video.loaded_sprites[node_data->item.sprite_index].text.processed_surface_index]->w,
					.h = app->video.loaded_surfaces[app->video.loaded_sprites[node_data->item.sprite_index].text.processed_surface_index]->h,
				};
				break;
			case G00_UI_MENU_CHILD_NODE_TYPE_LABEL:
				text_sprite_load_result = G00_VideoGenerateTextSprite(&app->video, app->game_state.primary_font_index, node_data->label.title, 13, &node_data->label.sprite_index);
				if (text_sprite_load_result < 0) {
					fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
					return -1;
				} if (text_sprite_load_result > 0) {
					fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
				}
				app->video.loaded_sprites[node_data->label.sprite_index].text.rect = (SDL_FRect) {
					.x = 100,
					//.y = app->video.config.screen_height - app->video.loaded_surfaces[app->video.loaded_sprites[text_sprite_index].surface_index]->h,
					.y = 300 + menu_y * (app->video.loaded_surfaces[app->video.loaded_sprites[node_data->label.sprite_index].text.processed_surface_index]->h * 1.5f),
					.w = app->video.loaded_surfaces[app->video.loaded_sprites[node_data->label.sprite_index].text.processed_surface_index]->w,
					.h = app->video.loaded_surfaces[app->video.loaded_sprites[node_data->label.sprite_index].text.processed_surface_index]->h,
				};
				break;
			default:
				// TODO issue warning
				break;
		}
		child = child->next;
		menu_y += 1;
	}

	return 0;
}

int G00_AppUpdate(struct G00_App* app) {
	if (G00_MemoryRetrieveIndex(&app->memory, "font-ui.ttf", &app->game_state.primary_font_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve font!\n");
		return -1;
	}

	int font_load_result = G00_VideoLoadFont(&app->video, app->memory.entries[app->game_state.primary_font_asset_index].len, app->memory.data + app->memory.entries[app->game_state.primary_font_asset_index].offset, 16.f, &app->game_state.primary_font_index);
	if (font_load_result < 0) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (font_load_result > 0) {
		fprintf(stdout, "Warning: Font loaded abnormally.\n");
	}

	G00_AppSetupMenu(app);

	if (app->mode == G00_APP_MODE_UI) {
		if (G00_MemoryRetrieveIndex(&app->memory, "menu-fg-parallax.png", &app->game_state.menu_fg_asset_index) < 0) {
			fprintf(stderr, "Unable to retrieve image!\n");
			return -1;
		}

		int fg_sprite_load_result = G00_VideoLoadImageSprite(
			&app->video,
			app->memory.entries[app->game_state.menu_fg_asset_index].len,
			app->memory.data + app->memory.entries[app->game_state.menu_fg_asset_index].offset,
			&app->game_state.menu_fg_sprite_index
		);
		if (fg_sprite_load_result < 0) {
			fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
			return -1;
		} if (fg_sprite_load_result > 0) {
			fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
		}

		G00_VideoDuplicateSprite(&app->video, app->game_state.menu_fg_sprite_index, &app->game_state.menu_fg_shadow0_sprite_index);
		app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.processing_nodes = &(struct G00_ListNode) {
			.data = &(struct G00_VideoFillColorProcessingNode) {
				.type = G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
				.color = (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0x00, .a = 0xFF },
			},
			.next = NULL,
		};

		G00_VideoDuplicateSprite(&app->video, app->game_state.menu_fg_sprite_index, &app->game_state.menu_fg_shadow1_sprite_index);
		app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.processing_nodes = &(struct G00_ListNode) {
			.data = &(struct G00_VideoFillColorProcessingNode) {
				.type = G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
				.color = (SDL_Color) { .r = 0xFF, .g = 0x00, .b = 0xFF, .a = 0xFF },
			},
			.next = NULL,
		};

		G00_VideoDuplicateSprite(&app->video, app->game_state.menu_fg_sprite_index, &app->game_state.menu_fg_shadow2_sprite_index);
		app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.processing_nodes = &(struct G00_ListNode) {
			.data = &(struct G00_VideoFillColorProcessingNode) {
				.type = G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
				.color = (SDL_Color) { .r = 0x00, .g = 0xFF, .b = 0xFF, .a = 0xFF },
			},
			.next = NULL,
		};

		G00_VideoDuplicateSprite(&app->video, app->game_state.menu_fg_sprite_index, &app->game_state.menu_fg_shadow3_sprite_index);
		app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.processing_nodes = &(struct G00_ListNode) {
			.data = &(struct G00_VideoFillColorProcessingNode) {
				.type = G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
				.color = (SDL_Color) { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF },
			},
			.next = NULL,
		};

		const float half_screen_x = app->video.config.screen_width / 2.f;
		const float half_screen_y = app->video.config.screen_height / 2.f;

		float base_fg_x = half_screen_x - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->w / 2.f);
		float base_fg_y = half_screen_y - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->h / 2.f);

		app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.rect = (SDL_FRect) {
			.x = base_fg_x,
			.y = base_fg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->h,
		};

		app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.rect = (SDL_FRect) {
			.x = base_fg_x,
			.y = base_fg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.processed_surface_index]->h,
		};

		app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.rect = (SDL_FRect) {
			.x = base_fg_x,
			.y = base_fg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.processed_surface_index]->h,
		};

		app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.rect = (SDL_FRect) {
			.x = base_fg_x,
			.y = base_fg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.processed_surface_index]->h,
		};

		app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.rect = (SDL_FRect) {
			.x = base_fg_x,
			.y = base_fg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.processed_surface_index]->h,
		};

		if (G00_MemoryRetrieveIndex(&app->memory, "menu-bg-parallax.png", &app->game_state.menu_bg_asset_index) < 0) {
			fprintf(stderr, "Unable to retrieve image!\n");
			return -1;
		}

		int bg_sprite_load_result = G00_VideoLoadImageSprite(
			&app->video,
			app->memory.entries[app->game_state.menu_bg_asset_index].len,
			app->memory.data + app->memory.entries[app->game_state.menu_bg_asset_index].offset,
			&app->game_state.menu_bg_sprite_index
		);
		if (bg_sprite_load_result < 0) {
			fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
			return -1;
		} if (bg_sprite_load_result > 0) {
			fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
		}

		float base_bg_x = half_screen_x - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->w / 2.f);
		float base_bg_y = half_screen_y - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->h / 2.f);

		app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.rect = (SDL_FRect) {
			.x = base_bg_x,
			.y = base_bg_y,
			.w = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->w,
			.h = app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->h,
		};
	}

	SDL_Event e;
	const float parallax_offset = 16.f;
	while (app->flags.force_exit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, "exit", "", app);
			}

			if (e.type == SDL_EVENT_KEY_DOWN) {
				if (app->mode == G00_APP_MODE_UI) {
					if (e.key.key == SDLK_UP) {
						G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, "do", "up", app);
					} else if (e.key.key == SDLK_DOWN) {
						G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, "do", "down", app);
					} else if (e.key.key == SDLK_RETURN) {
						if (app->ui.current_item != NULL && app->ui.current_item->base.type == G00_UI_MENU_CHILD_NODE_TYPE_ITEM) {
							G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, app->ui.current_item->item.script_commands, "", app); // TODO parse command string
						}
					} else if (e.key.key == SDLK_ESCAPE) {
						G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, "ui_menu_pop", "", app);
					}
				}
			}

			if (e.type == SDL_EVENT_MOUSE_MOTION) {
				if (app->mode == G00_APP_MODE_UI) {
					const float half_screen_x = app->video.config.screen_width / 2.f;
					const float half_screen_y = app->video.config.screen_height / 2.f;

					float base_fg_x = half_screen_x - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->w / 2.f);
					float base_fg_y = half_screen_y - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.processed_surface_index]->h / 2.f);

					float base_bg_x = half_screen_x - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->w / 2.f);
					float base_bg_y = half_screen_y - (app->video.loaded_surfaces[app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.processed_surface_index]->h / 2.f);

					app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.rect.x = base_bg_x + ((e.motion.x - half_screen_x) / half_screen_x * -parallax_offset);
					app->video.loaded_sprites[app->game_state.menu_bg_sprite_index].image.rect.y = base_bg_y + ((e.motion.y - half_screen_y) / half_screen_y * -parallax_offset);

					app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.rect.x = base_fg_x + ((e.motion.x - half_screen_x) / half_screen_x * parallax_offset);
					app->video.loaded_sprites[app->game_state.menu_fg_sprite_index].image.rect.y = base_fg_y + ((e.motion.y - half_screen_y) / half_screen_y * parallax_offset);

					app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.rect.x = base_fg_x + 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 32.f));
					app->video.loaded_sprites[app->game_state.menu_fg_shadow0_sprite_index].image.rect.y = base_fg_y + 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 64.f));

					app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.rect.x = base_fg_x - 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 64.f));
					app->video.loaded_sprites[app->game_state.menu_fg_shadow1_sprite_index].image.rect.y = base_fg_y - 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 32.f));

					app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.rect.x = base_fg_x + 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 32.f));
					app->video.loaded_sprites[app->game_state.menu_fg_shadow2_sprite_index].image.rect.y = base_fg_y - 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 32.f));

					app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.rect.x = base_fg_x - 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 64.f));
					app->video.loaded_sprites[app->game_state.menu_fg_shadow3_sprite_index].image.rect.y = base_fg_y + 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 64.f));
				}
			}
		}

		app->ticks = SDL_GetTicks();
		G00_AppSetupMenu(app);
		G00_VideoUpdate(&app->video, app->ticks);
	}

	return 0;
}

void G00_AppTeardown(struct G00_App* app) {
	G00_VideoTeardown(&app->video);
	TTF_Quit();
	SDL_Quit();
	G00_DatabaseTeardown(&app->db);
	G00_MemoryTeardown(&app->memory);
}

int main(int argc, char* argv[]) {
	struct G00_App app;
	enum G00_AppInitResult app_init_result = G00_AppInit(&app, argc, argv);
	if (app_init_result < 0) {
		return -1;
	}

	int app_update_result = G00_AppUpdate(&app);
	if (app_update_result < 0) {
		return app_update_result;
	}

	G00_AppTeardown(&app);

	return 0;
}
