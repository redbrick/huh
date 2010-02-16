/*
** File: g_list.c
** Desc: Implementation of a general-use list
** Auth: Cian Synnott <pooka@redbrick.dcu.ie>
** Date: Mon Nov  9 18:03:28 GMT 1998 
*/

#include <stdio.h>
#include <stdlib.h>

#include "g_list.h"

/* 
** Memory handling functions 
*/

/* The allocation function is passed a pointer to a function that returns
** 1 when the key passed matches whatever there is to be matched in the 
** the data passed, and 0 when it doesn't. For meaningful traversal */
g_list *g_listAlloc(int (*keyfunc)() ) {
	g_list *list;
	
	/* allocate memory in a careful manner */
	if ( !(list = (g_list *) malloc(sizeof(g_list))) ) {
		return NULL;
	}
       
	/* Set the head node equals null, and setup the tail node so that
	** it points to itself with next & prev (safest like that) */
	list->head = NULL;
	list->tail = g_nodeAlloc(NULL, NULL);
	list->tail->next = list->tail;
	list->tail->prev = list->tail;

	/* Set the function pointer to the passed function */
	list->keyfunc = keyfunc;

	return list;
}

/* Free an entire list */
int    g_listFree(g_list *list) {
	g_node *head;
	
	/* If the head's already null, just free the tail */
	if (list->head != NULL){

		/* Otherwise do the whole node-freeing thing */
		while (list->head != list->tail) {
			head = list->head->next;
			if (!g_nodeFree(list->head)) return 0;
			list->head = head;
		}
	}

	/* Finally free the tail */
	g_nodeFree(list->tail); 
	list->head = NULL;
	list->tail = NULL;
	list->keyfunc = NULL;

	/* Free the actual list memory */
	free(list);

	return 1;
}

/* Allocate a node, passing it the data to be held and a pointer to 
** the function to be used to free that data */
g_node *g_nodeAlloc(void *data, void (*freefunc)()) {
	g_node *node;
	
	/* Alloc */
	if (!(node = (g_node *) malloc(sizeof(g_node))) ) {
		return NULL;
	}

	/* Just fill in the blanks */
	node->next = NULL;
	node->prev = NULL;
	node->data = data;
	node->freefunc = freefunc;

	return node;
}

/* Free a node & it's data, being careful of nodes linked to themselves */
int     g_nodeFree(g_node *node) {
	
	/* This is a bit complex. It has to do with freeing things, different
	** ways of deleting the node depending on which links are pointing at 
	** itself. I have the algorithm somewhere, but I couldn't be bothered
	** reproducing it */
	if (node->next != node) {
		if (node->prev != node) {
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
		
		else
			node->next->prev = node->next;

	}
	
	else
		if (node->prev != node)
			node->prev->next = node->prev;

	node->next = NULL;
	node->prev = NULL;
	
	if (node->data)
		node->freefunc(node->data);
	node->data = NULL;
	
	node->freefunc = NULL;
	
	free(node);

	return 1;
}

/* 
** List handling functions 
*/

/* Put the passed piece of data at the beginning of the list */
int     g_listInsert(g_list *list, void *data, void (*freefunc)()) {
	g_node *node;      

	if (!(node = g_nodeAlloc(data, freefunc))) {
		return 0;
	}

	if (list->head == NULL) {
		list->head = node;
		node->next = list->tail;
		list->tail->prev = node;
		node->prev = node;
	}

	else {
		list->head->prev = node;
		node->prev = node;
		node->next = list->head;
		list->head = node;
	}

	return 1;
}

/* Put the passed piece of data at the end of the list */
int     g_listAppend(g_list *list, void *data, void (*freefunc)()) {
	g_node *node;

	if (!(node = g_nodeAlloc(data, freefunc))) {
		return 0;
	}

	if (list->head == NULL) {
		list->head = node;
		node->next = list->tail;
		list->tail->prev = node;
		node->prev = node;
	}
        
	else {
		list->tail->prev->next = node;
		node->next = list->tail;
		node->prev = list->tail->prev;
		list->tail->prev = node;
	}

	return 1;
}

/* If NULL is passed, continue traversing current list. Else traverse the
** list passed. Returns the next item of data in the list, or NULL at end */
void   *g_listTraverse(g_list *list) {
	static g_node *s_node;

	if (list == NULL) {
		if (s_node == s_node->next) return NULL;
		else s_node = s_node->next;
		return s_node->data;
	}

	else {
		if (list->head) {
			s_node = list->head;
			return s_node->data;
		}
		else return NULL;
	}
}

/* Traverses the list & gets the first piece of data for which keyfunc() 
** returns 1 when passed key. */
void   *g_listGet(g_list *list, void *key) {
	g_node *node = list->head;
        
	while (node != node->next) {
		if (list->keyfunc(key, node->data)) 
			return node->data;
		node = node->next;
	}

	return NULL;
}

/* Traverses the list & deletes the first piece of data for which keyfunc()
** returns 1 when passed key. */
int     g_listDel(g_list *list, void *key) {
	g_node *node = list->head;
        
	while (node != node->next) {
		if (list->keyfunc(key, node->data)) {
			node = node->next;
			if (!g_nodeFree(node)) return 0;
			return 1;
		}
		else node = node->next;
	}

	return 0;
}

/* 
** LIFO implementation 
*/

/* Should be a define, but I like it better like this */
int     g_listPush(g_list *list, void *data, void (*freeFunc)()) {
	return g_listInsert(list, data, freeFunc);
}

/* Have to cheat with the node-freeing here to preserve the data ... */
void   *g_listPop(g_list *list) {
	g_node *node;
	void *data;
       
	if (list->head == NULL) return NULL;
	
	node = list->head;
	data = node->data;
       
	if (node->next != list->tail) {
		node->next->prev = node->next;
		list->head = node->next;
	}

	else {
		list->head = NULL;
		list->tail->prev = list->tail;
	}

	node->next = NULL;
	node->prev = NULL;
	node->data = NULL;
	node->freefunc = NULL;
	free(node);

	return data;
}

/* 
** FIFO implementation
*/
   
/* Same again, 'cept use append, and wrap the Pop function for the Q */
int     g_listQPut(g_list *list, void *data, void (*freeFunc)()) {
	return g_listAppend(list, data, freeFunc);
}

/* Macros, macros, all of these should be macros... */
void   *g_listQGet(g_list *list) {
	return g_listPop(list);
}        
