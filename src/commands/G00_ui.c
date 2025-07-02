#include <stdio.h>

#include "../G00_commands.h"

int G00_XCommandUIMsgKey(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	struct G00_MessageEntry new_entry;
	if (G00_CommandParseArgs(args, arg_defs, NULL, new_entry.key, new_entry.fallback_value) < 0) {
		return -1;
	}

	struct G00_ListNode* added_entry;
	G00_ListAppend(&ui->messages, sizeof(struct G00_MessageEntry), &new_entry, &added_entry);
	struct G00_MessageEntry* added_entry_data = added_entry->data;
	fprintf(stdout, "Registered message key %s, fallback %s\n", added_entry_data->key, added_entry_data->fallback_value);
	return 0;
}

int G00_XCommandUIMenu(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	struct G00_UIMenuNode new_node = {
		.type = G00_UI_NODE_TYPE_MENU,
		.children = NULL,
	};

	if (G00_CommandParseArgs(args, arg_defs, NULL, new_node.label) < 0) {
		return -1;
	}

	struct G00_ListNode* added_entry;
	G00_ListAppend(&ui->menus, sizeof(struct G00_UIMenuNode), &new_node, &added_entry);
	struct G00_UIMenuNode* added_entry_data = added_entry->data;
	fprintf(stdout, "Registered menu %s\n", added_entry_data->label);
	ui->current_menu = added_entry_data;
	return 0;
}

int G00_XCommandUIItem(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	if (ui->current_menu == NULL) {
		return -2;
	}

	union G00_UIMenuChildNode new_item = {
		.item = {
			.type = G00_UI_NODE_TYPE_ITEM,
		}
	};

	if (G00_CommandParseArgs(args, arg_defs, NULL, new_item.item.script_commands) < 0) {
		return -1;
	}

	struct G00_ListNode* added_entry;
	G00_ListAppend(&ui->current_menu->children, sizeof(union G00_UIMenuChildNode), &new_item, &added_entry);
	union G00_UIMenuChildNode* added_entry_data = added_entry->data;
	fprintf(stdout, "Added item \"%s\" in menu %s\n", added_entry_data->item.script_commands, ui->current_menu->label);
	ui->current_item = added_entry_data;
	return 0;
}

int G00_XCommandUITitle(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	if (ui->current_item == NULL) {
		return -2;
	}

	if (G00_CommandParseArgs(args, arg_defs, NULL, ui->current_item->item.title) < 0) {
		return -1;
	}

	return 0;
}

int G00_XCommandUIDescriptionText(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	if (G00_CommandParseArgs(args, arg_defs, NULL, ui->current_item->item.description_text) < 0) {
		return -1;
	}

	return 0;
}

int G00_XCommandUILabel(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	if (ui->current_menu == NULL) {
		return -2;
	}

	union G00_UIMenuChildNode new_item = {
		.label = {
			.type = G00_UI_NODE_TYPE_LABEL,
		},
	};

	if (G00_CommandParseArgs(args, arg_defs, NULL) < 0) {
		return -1;
	}

	struct G00_ListNode* added_entry;
	G00_ListAppend(&ui->current_menu->children, sizeof(union G00_UIMenuChildNode), &new_item, &added_entry);
	union G00_UIMenuChildNode* added_entry_data = added_entry->data;
	fprintf(stdout, "Added label in menu %s\n", ui->current_menu->label);
	ui->current_item = added_entry_data;
	return 0;
}

int G00_XCommandUIMenuPush(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	char menu_id[255];
	if (G00_CommandParseArgs(args, arg_defs, NULL, menu_id) < 0) {
		return -1;
	}

	if (ui->history_stack_index == 32) {
		// overflow
		return -2;
	}

	struct G00_ListNode** p = &ui->menus;
	while (*p != NULL) {
		struct G00_UIMenuNode* data = (*p)->data;
		if (!strcmp(data->label, menu_id)) {
			ui->history_stack[ui->history_stack_index] = data;
			fprintf(stdout, "Pushed history item %s\n", ui->history_stack[ui->history_stack_index]->label);
			ui->history_stack_index += 1;
			return 0;
		}
		*p = (*p)->next;
	}

	return 1;
}

int G00_XCommandUIMenuPop(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	if (ui->history_stack_index == 0) {
		// underflow
		return -1;
	}

	ui->history_stack_index -= 1;
	fprintf(stdout, "Popped history item %s\n", ui->history_stack[ui->history_stack_index]->label);
	ui->history_stack[ui->history_stack_index] = NULL;

	return 0;
}

int G00_XCommandUIMenuShowStart(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_UIState* ui) {
	// TODO
	return 0;
}
