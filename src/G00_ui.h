#ifndef G00_UI_H
#define G00_UI_H

#include "G00_list.h"

struct G00_MessageEntry {
	char key[255];
	char fallback_value[255];
};

enum G00_UIMenuChildNodeType : unsigned char {
	G00_UI_MENU_CHILD_NODE_TYPE_BASE,
	G00_UI_MENU_CHILD_NODE_TYPE_LABEL,
	G00_UI_MENU_CHILD_NODE_TYPE_ITEM,
};

struct G00_UIMenuChildBaseNode {
	enum G00_UIMenuChildNodeType type;
};

struct G00_UIMenuChildLabelNode {
	enum G00_UIMenuChildNodeType type;
	unsigned int sprite_index;
	char title[255];
};

struct G00_UIMenuChildItemNode {
	enum G00_UIMenuChildNodeType type;
	unsigned int sprite_index;
	char title[255];
	char script_commands[255];
	char description_text[255];
};

union G00_UIMenuChildNode {
	struct G00_UIMenuChildBaseNode base;
	struct G00_UIMenuChildLabelNode label;
	struct G00_UIMenuChildItemNode item;
};

struct G00_UIMenuParentNode {
	char label[255];
	struct G00_ListNode* children;
};

struct G00_UIState {
	struct G00_ListNode* menus;

	struct G00_UIMenuParentNode* current_menu;
	union G00_UIMenuChildNode* current_item;

	struct G00_ListNode* messages;
	unsigned char history_stack_index;
	struct G00_UIMenuParentNode* history_stack[32];
};

int G00_UIInit(struct G00_UIState*);

#endif
