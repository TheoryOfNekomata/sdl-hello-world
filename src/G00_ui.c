#include "G00_ui.h"
#include <string.h>

int G00_UIInit(struct G00_UIState* state) {
	state->messages = NULL;
	state->menus = NULL;
	state->current_item = NULL;
	state->current_menu = NULL;
	state->history_stack_index = 0;
	return 0;
}
