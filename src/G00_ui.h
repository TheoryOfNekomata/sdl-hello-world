#ifndef G00_UI_H
#define G00_UI_H

struct G00_ListNode {
	void* data;
	struct G00_ListNode* next;
};

struct G00_MessageEntry {
	char key[255];
	char fallback_value[255];
};

enum G00_UINodeType : unsigned char {
	G00_UI_NODE_TYPE_MENU,
	G00_UI_NODE_TYPE_LABEL,
	G00_UI_NODE_TYPE_ITEM,
};

struct G00_UILabelNode {
	enum G00_UINodeType type;
	char title[255];
};

struct G00_UIItemNode {
	enum G00_UINodeType type;
	char title[255];
	char script_commands[255];
	char description_text[255];
};

union G00_UIMenuChildNode {
	struct G00_UILabelNode label;
	struct G00_UIItemNode item;
};

struct G00_UIMenuNode {
	enum G00_UINodeType type;
	char label[255];
	struct G00_ListNode* children;
};

struct G00_UIState {
	struct G00_ListNode* menus;

	struct G00_UIMenuNode* current_menu;
	union G00_UIMenuChildNode* current_item;

	struct G00_ListNode* messages;
	unsigned char history_stack_index;
	struct G00_UIMenuNode* history_stack[32];
};

int G00_UIInit(struct G00_UIState*);

int G00_ListAppend(struct G00_ListNode**, size_t, void*, struct G00_ListNode** out0);

#endif
