#pragma once

/*
 * given a custom struct type on the form
 *
 * struct mynode {
 * 	...
 * } mynode;
 *
 * and changing it to
 *
 * struct mynode {
 * 	...
 * 	SLL_LINK(mynode);
 * 	...
 * } mynode;
 *
 * together with the following
 *
 * SLL_DECLS(mysll, mynode, mylist);
 *
 * where header stuff is appropriate, and
 *
 * SLL_DEFS(mysll, mynode, mylist, nodefree);
 *
 * where source stuff is appropriate, will give you a singly linked list implementation with
 * mylist as the type keeping track of the first and last mynode node as well as the length of
 * the list, and the functions
 *
 * void    mysll_lclear(mylist *list)                  // clears the list so that it appears as empty (naive, no deallocation is done)
 * void    mysll_lnclear(mynode *node)                 // clears any link data from a node (naive, no deallocation is done)
 * size_t  mysll_lsize(const mylist *list)             // returns the number of elements in the list
 * void    mysll_lpushback(mylist *list, mynode *node) // appends a mynode element to the list
 * mynode *mysll_lpopfront(mylist *list)               // removes and returns the first element of the list (or NULL)
 * void    mysll_lfree(mylist *list)                   // empties the list and calls nodefree on all nodes
 *
 * ITERATOR FUNCTIONS
 *
 * If you make use of the SLL_ITER_DECLS and SLL_ITER_DEFS with parameters (mysll, mynode, mylist, myiter) additionally, you get
 *
 * void    mysll_istart(myiter *iter, mylist *list)    // initialize a forward iterator for the list at its start
 * mynode *mysll_iget(myiter *iter)                    // get the current node of the iterator
 * void    mysll_inext(myiter *iter)                   // set the iterator to point at the next node in the list
 * bool    mysll_iisend(const myiter *iter)            // returns true if the iterator has reached the end of the list (current node is NULL)
 * mynode *mysll_ipop(myiter *iter)                    // removes the current pointed-at node from the list and returns it (or NULL)
 *
 * You can also make use of the macro SLL_ITER_START, as in "someiter it = SLL_ITER_START(&somelist)" to statically initialize
 * an iterator e.g. in the initialization field of a for loop.
 *
 * MEMORY POOL FUNCTIONS
 *
 * If you make use of the SLL_POOL_DECLS and SLL_POOL_DEFLS with parameters (mysll, mynode, mylist, mypool) additionally, you get
 *
 * void    mysll_pclear(mypool *pool)                // initializes an empty pool
 * mynode *mysll_pget(mypool *pool)                  // returns the first node in the pool or callocs a new one iff the pool is empty
 * mynode *mysll_pgetm(mypool *pool, bool *isnew)    // returns the first node in the pool or mallocs a new one iff the pool is empty
 *                                                   // isnew is set to true or false depending on whether a new node was allocated or not
 * void    mysll_preturn(mypool *pool, mynode *node) // puts the node back in the pool
 * void    mysll_pfree(mypool *pool)                 // empties the pool and frees memory
 *
 */

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#ifndef CONCAT_
#define CONCAT_(a, b) a ## _ ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_(a, b)
#endif

// in-type data addition
#define SLL_LINK(node_type) struct node_type *sll_link_next

// header declarations
#define SLL_DECLS(function_prefix, node_type, list_type) \
	typedef struct { /*{{{*/ \
		node_type *first; \
		node_type *last; \
		size_t n; \
	} list_type; /*}}}*/ \
	void       CONCAT(function_prefix, lclear)   (list_type *list); \
	void       CONCAT(function_prefix, lnclear)  (node_type *node); \
	size_t     CONCAT(function_prefix, lsize)    (const list_type *list); \
	void       CONCAT(function_prefix, lpushback)(list_type *list, node_type *node); \
	node_type *CONCAT(function_prefix, lpopfront)(list_type *list); \
	void       CONCAT(function_prefix, lfree)    (list_type *list)

#define SLL_ITER_DECLS(function_prefix, node_type, list_type, iterator_type) \
	typedef struct { /*{{{*/\
		list_type *list; \
		node_type *prev; \
		node_type *current; \
		node_type *next; \
	} iterator_type; /*}}}*/\
	void       CONCAT(function_prefix, istart)(iterator_type *iter, list_type *list); \
	node_type *CONCAT(function_prefix, iget)  (iterator_type *iter); \
	void       CONCAT(function_prefix, inext) (iterator_type *iter); \
	bool       CONCAT(function_prefix, iisend)(const iterator_type *iter); \
	node_type *CONCAT(function_prefix, ipop)  (iterator_type *iter)

#define SLL_POOL_DECLS(function_prefix, node_type, list_type, pool_type) \
	typedef list_type pool_type; \
	void        CONCAT(function_prefix, pclear) (pool_type *pool); \
	node_type  *CONCAT(function_prefix, pget)   (pool_type *pool); \
	void        CONCAT(function_prefix, preturn)(pool_type *pool, node_type *node); \
	void        CONCAT(function_prefix, pfree)  (pool_type *pool)

// definitions

#define SLL_ISTART(_list) { .list=(_list), .prev=NULL, .current=(_list)->first!=NULL?(_list)->first:NULL, .next=(_list)->first!=NULL?(_list)->first->sll_link_next:NULL }

#define SLL_DEFS(function_prefix, node_type, list_type, node_free_func) \
	void CONCAT(function_prefix, lclear)(list_type *list) { /*{{{*/ \
		assert(list != NULL); \
		list->first = NULL; \
		list->last = NULL; \
		list->n = 0; \
	} /*}}}*/ \
	void CONCAT(function_prefix, lnclear)(node_type *node) { /*{{{*/ \
		assert(node != NULL); \
		node->sll_link_next = NULL; \
	} /*}}}*/ \
	size_t CONCAT(function_prefix, lsize)(const list_type *list) { /*{{{*/ \
		assert(list != NULL); \
		return list->n; \
	} /*}}}*/ \
	void CONCAT(function_prefix, lpushback)(list_type *list, node_type *node) { /*{{{*/ \
		assert(list != NULL); \
		if (list->n == 0) { \
			list->first = node; \
			list->last = node; \
			list->n = 1; \
		} \
		else { \
			list->last->sll_link_next = node; \
			list->last = node; \
			++list->n; \
		} \
	} /*}}}*/ \
	node_type *CONCAT(function_prefix, lpopfront)(list_type *list) { /*{{{*/ \
		assert(list != NULL); \
		if (list->n == 0) { return NULL; } \
		node_type *node = list->first; \
		list->first = node->sll_link_next; \
		--list->n; \
		if (list->first == NULL) { list->last = NULL; } \
		node->sll_link_next = NULL; \
		return node; \
	} /*}}}*/ \
	void CONCAT(function_prefix, lfree)(list_type *list) { /*{{{*/ \
		while (CONCAT(function_prefix, lsize)(list) > 0) { \
			node_type * node = CONCAT(function_prefix, lpopfront)(list); \
			node_free_func(node); \
		} \
	} /*}}}*/

#define SLL_ITER_DEFS(function_prefix, node_type, list_type, iterator_type) \
	void CONCAT(function_prefix, istart)(iterator_type *iter, list_type *list) { /*{{{*/ \
		assert(iter != NULL); \
		assert(list != NULL); \
		iter->list = list; \
		iter->prev = NULL; \
		iter->current = list->first; \
		if (iter->current != NULL) { \
			iter->next = iter->current->sll_link_next; \
		} \
	} /*}}}*/ \
	node_type *CONCAT(function_prefix, iget)(iterator_type *iter) { /*{{{*/ \
		assert(iter != NULL); \
		return iter->current; \
	} /*}}}*/ \
	void CONCAT(function_prefix, inext)(iterator_type *iter) { /*{{{*/ \
		assert(iter != NULL); \
		if (iter->current != NULL) { \
			iter->prev = iter->current; \
		} \
		iter->current = iter->next; \
		if (iter->current != NULL) { \
			iter->next = iter->current->sll_link_next; \
		} \
	} /*}}}*/ \
	bool CONCAT(function_prefix, iisend)(const iterator_type *iter) { /*{{{*/ \
		assert(iter != NULL); \
		return iter->list->last == iter->prev && iter->current == NULL && iter->next == NULL; \
	} /*}}}*/ \
	node_type *CONCAT(function_prefix, ipop)(iterator_type *iter) { /*{{{*/ \
		assert(iter != NULL); \
		node_type *node = iter->current; \
		if (node != NULL) { \
			if (iter->current == iter->list->first) { \
				iter->list->first = iter->next; \
			} \
			if (iter->current == iter->list->last) {\
				iter->list->last = iter->prev; \
			} \
			node->sll_link_next = NULL; \
			iter->current = NULL; \
			--iter->list->n; \
		} \
		if (iter->prev != NULL) { \
			iter->prev->sll_link_next = iter->next; \
		} \
		return node; \
	} /*}}}*/

#define SLL_POOL_DEFS(function_prefix, node_type, list_type, pool_type) \
	void CONCAT(function_prefix, pclear)(pool_type *pool) { /*{{{*/ \
		CONCAT(function_prefix, lclear)((list_type*)pool); \
	} /*}}}*/ \
	node_type *CONCAT(function_prefix, pget)(pool_type *pool) { /*{{{*/ \
		assert(pool != NULL); \
		node_type *ret = CONCAT(function_prefix, lpopfront)((list_type*)pool); \
		if (ret == NULL) { \
			ret = calloc(1, sizeof(node_type)); \
		} \
		return ret; \
	} /*}}}*/ \
	node_type *CONCAT(function_prefix, pgetm)(pool_type *pool, bool *isnew) { /*{{{*/ \
		assert(pool != NULL); \
		assert(isnew != NULL); \
		node_type *ret = CONCAT(function_prefix, lpopfront)((list_type*)pool); \
		*isnew = false; \
		if (ret == NULL) { \
			ret = malloc(sizeof(node_type)); \
			*isnew = true; \
		} \
		return ret; \
	} /*}}}*/ \
	void CONCAT(function_prefix, preturn)(pool_type *pool, node_type *node) { /*{{{*/ \
		assert(pool != NULL); \
		assert(node != NULL); \
		CONCAT(function_prefix, lpushback)((list_type*)pool, node); \
	} /*}}}*/ \
	void CONCAT(function_prefix, pfree)(pool_type *pool) { /*{{{*/ \
		assert(pool != NULL); \
		CONCAT(function_prefix, lfree)((list_type*)pool); \
	} /*}}}*/
