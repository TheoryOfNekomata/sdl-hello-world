#ifndef G00_LIST_H
#define G00_LIST_H

#include <stdlib.h>

struct G00_ListNode {
	void* data;
	struct G00_ListNode* next;
};

int G00_ListAppend(struct G00_ListNode**, size_t, void*, struct G00_ListNode** out0);

#endif //G00_LIST_H
