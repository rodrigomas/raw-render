#ifndef _LINKED_LIST_
#define _LINKED_LIST_

#include "algebra.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _LinkedListNode
{
	struct _LinkedListNode * next;

	void *value;
};

typedef struct _LinkedListNode * LinkedListNode;

struct _LinkedList
{
	long size;
	struct _LinkedListNode *head;
};

typedef struct _LinkedList * LinkedList;

LinkedList lstCreate();

void lstDestroy(LinkedList lst);

LinkedListNode lstAddValue(LinkedList lst, void * val, BOOL onHead );

long lstGetSize(LinkedList lst);

LinkedListNode lstGetHead(LinkedList lst);

LinkedListNode lstInsertValueAfter(LinkedList lst, void * val, LinkedListNode prev);

LinkedListNode lstSearch(LinkedList lst, void * val, LinkedListNode prev);

void* lstDelete(LinkedList lst, LinkedListNode val);

#ifdef __cplusplus
}
#endif

#endif
