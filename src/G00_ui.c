#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "G00_ui.h"

int G00_UIInit(struct G00_UIState* state) {
	state->messages = NULL;
	state->menus = NULL;
	state->current_item = NULL;
	state->current_menu = NULL;
	return 0;
}

int G00_ListAppend(struct G00_ListNode** root, size_t len, void* data, struct G00_ListNode** out0_new_node) {
	struct G00_ListNode* p;

	if (*root == NULL) {
		*root = malloc(sizeof(struct G00_ListNode));
		if (*root == NULL) {
			return -1;
		}
		p = *root;
	} else {
		p = *root;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = malloc(sizeof(struct G00_ListNode));
		if (p->next == NULL) {
			return -1;
		}
		p = p->next;
	}

	p->data = malloc(len);
	if (p->data == NULL) {
		return -1;
	}
	p->next = NULL;
	memcpy(p->data, data, len);
	*out0_new_node = p;
	return 0;
}
