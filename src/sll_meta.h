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
 * SLL_DEFS(mysll, mynode, mylist);
 *
 * where source stuff is appropriate, will give you a singly linked list implementation with
 * mylist as the type keeping track of the first and last mynode node as well as the length of
 * the list, and the functions
 * void    mysll_list_clear(mylist *list)                  // clears the list so that it appears as empty (naive, no deallocation is done)
 * void    mysll_list_node_clear(mynode *node)             // clears any link data from a node (naive, no deallocation is done)
 * size_t  mysll_list_size(const mylist *list)             // returns the number of elements in the list
 * void    mysll_list_pushback(mylist *list, mynode *node) // appends a mynode element to the list
 * mynode *mysll_list_popfront(mylist *list)               // removes and returns the first element of the list (or NULL)
 *
 * ITERATOR FUNCTIONS
 *
 * If you make use of the SLL_ITER_DECLS and SLL_ITER_DEFS with parameters (mysll, mynode, mylist, myiter) additionally, you get
 *
 * void    mysll_iter_start(myiter *iter, mylist *list)    // initialize a forward iterator for the list at its start
 * mynode *mysll_iter_get(myiter *iter)                    // get the current node of the iterator
 * void    mysll_iter_next(myiter *iter)                   // set the iterator to point at the next node in the list
 * bool    mysll_iter_isend(const myiter *iter)            // returns true if the iterator has reached the end of the list (current node is NULL)
 * mynode *mysll_iter_pop(myiter *iter)                    // removes the current pointed-at node from the list and returns it (or NULL)
 *
 * MEMORY POOL FUNCTIONS
 *
 * If you make use of the SLL_POOL_DECLS and SLL_POOL_DEFLS with parameters (mysll, mynode, mylist, mypool) additionally, you get
 *
 * void    mysll_pool_clear(mypool *pool)                // initializes an empty pool
 * mynode *mysll_pool_get(mypool *pool)                  // returns the first node in the pool or callocs a new one iff the pool is empty
 * void    mysll_pool_return(mypool *pool, mynode *node) // puts the node back in the pool
 *
 */

#include <stddef.h>
#include <assert.h>

#define CONCAT_(a, b) a ## _ ## b
#define CONCAT(a, b) CONCAT_(a, b)

// in-type data addition
#define SLL_LINK(node_typename) struct node_typename *sll_link_next

// header declarations
#define SLL_DECLS(function_prefix, node_typename, list_typename) \
	typedef struct { /*{{{*/ \
		node_typename *first; \
		node_typename *last; \
		size_t n; \
	} list_typename; /*}}}*/ \
	void           CONCAT(function_prefix, list_clear)     (list_typename *list); \
	void           CONCAT(function_prefix, list_node_clear)(node_typename *node); \
	size_t         CONCAT(function_prefix, list_size)      (const list_typename *list); \
	void           CONCAT(function_prefix, list_pushback)  (list_typename *list, node_typename *node); \
	node_typename *CONCAT(function_prefix, list_popfront)  (list_typename *list)

#define SLL_ITER_DECLS(function_prefix, node_typename, list_typename, iterator_typename) \
	typedef struct { /*{{{*/\
		list_typename *list; \
		node_typename *prev; \
		node_typename *current; \
	} iterator_typename; /*}}}*/\
	void           CONCAT(function_prefix, iter_start)(iterator_typename *iter, list_typename *list); \
	node_typename *CONCAT(function_prefix, iter_get)  (iterator_typename *iter); \
	void           CONCAT(function_prefix, iter_next) (iterator_typename *iter); \
	bool           CONCAT(function_prefix, iter_isend)(const iterator_typename *iter); \
	node_typename *CONCAT(function_prefix, iter_pop)  (iterator_typename *iter)

#define SLL_POOL_DECLS(function_prefix, list_typename, node_typename, pool_typename) \
	typedef list_typename pool_typename; \
	node_typename *CONCAT(function_prefix, pool_get)   (pool_typename *pool); \
	void           CONCAT(function_prefix, pool_return)(pool_typename *pool, node_typename *node)

// definitions
#define SLL_DEFS(function_prefix, node_typename, list_typename, iterator_typename) \
	void CONCAT(function_prefix, list_clear)(list_typename *list) { /*{{{*/ \
		assert(list != NULL); \
		list->first = NULL; \
		list->last = NULL; \
		list->n = 0; \
	} /*}}}*/ \
	void CONCAT(function_prefix, list_node_clear)(node_typename *node) { /*{{{*/ \
		assert(node != NULL); \
		node->sll_link_next = NULL; \
	} /*}}}*/ \
	size_t CONCAT(function_prefix, list_size)(const list_typename *list) { /*{{{*/ \
		assert(list != NULL); \
		return list->n; \
	} /*}}}*/ \
	void CONCAT(function_prefix, list_pushback)(list_typename *list, node_typename *node) { /*{{{*/ \
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
	node_typename *CONCAT(function_prefix, list_popfront)(list_typename *list) { /*{{{*/ \
		assert(list != NULL); \
		if (list->n == 0) { return NULL; } \
		node_typename *node = list->first; \
		list->first = node->sll_link_next; \
		--list->n; \
		if (list->first == NULL) { list->last = NULL; } \
		node->sll_link_next = NULL; \
		return node; \
	} /*}}}*/ \

#define SLL_ITER_DEFS(function_prefix, node_typename, list_typename, iterator_typename) \
	void CONCAT(function_prefix, iter_start)(iterator_typename *iter, list_typename *list) { /*{{{*/ \
		assert(iter != NULL); \
		assert(list != NULL); \
		iter->list = list; \
		iter->prev = NULL; \
		iter->current = list->first; \
	} /*}}}*/ \
	node_typename *CONCAT(function_prefix, iter_get)(iterator_typename *iter) { /*{{{*/ \
		assert(iter != NULL); \
		return iter->current; \
	} /*}}}*/ \
	void CONCAT(function_prefix, iter_next)(iterator_typename *iter) { /*{{{*/ \
		assert(iter != NULL); \
		if (iter->current != NULL) { \
			iter->prev = iter->current; \
			iter->current = iter->current->sll_link_next; \
		} \
		else { \
			iter->prev = NULL; \
		} \
	} /*}}}*/ \
	bool CONCAT(function_prefix, iter_isend)(iterator_typename *iter) { /*{{{*/ \
		assert(iter != NULL); \
		return iter->current == NULL; \
	} /*}}}*/ \
	node_typename *CONCAT(function_prefix, iter_pop)(iterator_typename *iter) { /*{{{*/ \
		assert(iter != NULL); \
		if (iter->current != NULL) { \
			if (iter->current == iter->list->first) { \
				iter->list->first = iter->current->sll_link_next; \
			} \
			if (iter->current == iter->list->last) {\
				iter->list->last = iter->prev; \
			} \
			--iter->list.n; \
		} \
		return iter->current; \
	} /*}}}*/

#define SLL_POOL_DEFS(function_prefix, node_typename, list_typename, pool_typename) \
	node_typename *CONCAT(function_prefix, pool_get)(pool_typename *pool) { /*{{{*/ \
		assert(pool != NULL); \
		node_typename *ret = CONCAT(function_prefix, popfront)((list_typename*)pool); \
		if (ret == NULL) { \
			ret = calloc(1, sizeof(node_typename)); \
		} \
		return ret; \
	} /*}}}*/ \
	void CONCAT(function_prefix, pool_return)(pool_typename *pool, node_typename *node) { /*{{{*/ \
		assert(pool != NULL); \
		assert(node != NULL); \
		CONCAT(function_prefix, pushback)((list_typename*)pool, node); \
	} /*}}}*/
