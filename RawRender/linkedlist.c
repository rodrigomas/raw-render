#include "linkedlist.h"

LinkedList lstCreate()
{
	LinkedList lst = (struct _LinkedList *)malloc(sizeof(struct _LinkedList));

	lst->size = 0;
	lst->head = NULL;

	return lst;
}

long lstGetSize(LinkedList lst)
{
	return lst->size;
}

LinkedListNode lstGetHead(LinkedList lst)
{
	return lst->head;
}

void lstDestroy(LinkedList lst)
{
	LinkedListNode cur = lst->head;
	LinkedListNode local = NULL;

	while (cur != NULL)
	{
		local = cur;
		cur = cur->next;

		free(local);		
	}

	free(lst);
}

LinkedListNode lstAddValue(LinkedList lst, void * val, BOOL onHead)
{
	LinkedListNode cur = lst->head;
	LinkedListNode last = NULL;
	LinkedListNode node = NULL;	

	if (onHead == FALSE)
	{
		while (cur != NULL)
		{
			last = cur;
			cur = cur->next;
		}

		if (last != NULL)
		{
			node = (struct _LinkedListNode *)malloc(sizeof(struct _LinkedListNode));
			node->next = last->next;
			node->value = val;

			last->next = node;

			lst->size++;

			if (lst->head == NULL)
			{
				lst->head = node;
			}
		}		
	}
	else
	{
		node = (struct _LinkedListNode *)malloc(sizeof(struct _LinkedListNode));
		node->value = val;

		if (lst->head == NULL)
		{
			lst->head = node;
			node->next = NULL;
		}
		else
		{			
			node->next = lst->head;
			lst->head = node;			
		}
	
		lst->size++;		
	}

	return node;
}

LinkedListNode lstInsertValueAfter(LinkedList lst, void * val, LinkedListNode prev)
{
	LinkedListNode node = NULL;

	if (prev != NULL)
	{
		node = (struct _LinkedListNode *)malloc(sizeof(struct _LinkedListNode));
		node->next = prev->next;
		node->value = val;

		prev->next = node;
	}

	return node;
}

LinkedListNode lstSearch(LinkedList lst, void * val, LinkedListNode prev)
{
	LinkedListNode cur = prev;

	if (cur == NULL) cur = lst->head;

	while (cur != NULL)
	{
		if (cur->value == val)
			return cur;

		cur = cur->next;
	}

	return NULL;
}

void* lstDelete(LinkedList lst, LinkedListNode val)
{
	LinkedListNode cur = lst->head;
	LinkedListNode prev = NULL;
	void *d = val->value;

	while (cur != NULL)
	{
		prev = cur;

		if (cur == val)
			break;

		cur = cur->next;
	}

	if (prev != NULL)
	{
		prev->next = val->next;
	}
	
	if (val == lst->head)
	{
		lst->head = val->next;
	}

	free(val);

	return d;
}