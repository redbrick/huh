#ifndef _g_list_h_
#define _g_list_h_

/*
** File: g_list.h
** Desc: Implementation of a general-use list
**       At the time I wrote this, I didn't know that g_* was the glib way of
**       doing things. Woops. :o)
** Auth: Cian Synnott <pooka@redbrick.dcu.ie>
** Date: Mon Nov  9 18:03:53 GMT 1998 
*/

/* The node struct */
typedef struct _g_node_ {
	void *data;
	struct _g_node_ *next;
	struct _g_node_ *prev;
	void           (*freefunc)(void *data);
} g_node;

/* The list struct */
typedef struct _g_list_ {
	g_node *head;
	g_node *tail;
	int   (*keyfunc)(void *key, void *data);
} g_list;

/* 
** Memory handling functions 
*/

/* The allocation function is passed a pointer to a function that returns
** 1 when the key passed matches whatever there is to be matched in the 
** the data passed, and 0 when it doesn't. For meaningful traversal */
g_list *g_listAlloc(int (*keyfunc)(void *key, void *data) );
int     g_listFree(g_list *list);

/* Allocate a node, passing it the data to be held and a pointer to the
** function to be used to free that data */
g_node *g_nodeAlloc(void *data, void (*freefunc)());
int     g_nodeFree(g_node *node);

/* 
** List handling functions 
*/

/* Put the passed piece of data at the beginning of the list */
int     g_listInsert(g_list *list, void *data, void (*freefunc)());

/* Put the passed piece of data at the end of the list */
int     g_listAppend(g_list *list, void *data, void (*freefunc)());

/* If NULL is passed, continue traversing current list. Else traverse the
** list passed. Returns the next item of data in the list, or NULL at end */
void   *g_listTraverse(g_list *list);

/* Traverses the list & gets the first piece of data for which keyfunc() 
** returns 1 when passed key. */
void   *g_listGet(g_list *list, void *key);

/* Traverses the list & deletes the first piece of data for which keyfunc()
** returns 1 when passed key. */
int     g_listDel(g_list *list, void *key);

/* LIFO implementation */
int     g_listPush(g_list *list, void *data, void (*freeFunc)());
void   *g_listPop(g_list *list);

/* FIFO implementation */
int     g_listQPut(g_list *list, void *data, void (*freeFunc)());
void   *g_listQGet(g_list *list);

#endif
