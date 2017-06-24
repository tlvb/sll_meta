#include <stdlib.h>
#include <stdio.h>
#include "sll_meta.h"

typedef struct mynode {
	SLL_LINK(mynode);
	int id;
} mynode;

void printnode(const mynode *n, const char *note);

SLL_DECLS(mysll, mynode, mylist);
SLL_ITER_DECLS(mysll, mynode, mylist, myiter);
SLL_POOL_DECLS(mysll, mynode, mylist, mypool);

SLL_DEFS(mysll, mynode, mylist, free);
SLL_ITER_DEFS(mysll, mynode, mylist, myiter);
SLL_POOL_DEFS(mysll, mynode, mylist, mypool);

void printnode(const mynode *n, const char *note) {
	if (n->id != 0) {
		printf("node with id %d%s\n", n->id, note);
	}
	else {
		printf("uninitialized node%s\n", note);
	}
}

int main(void) {

	mypool pool = {0};
	mylist list = {0};
	myiter iter = {0};

	printf("-\ngetting items from the pool, pool is empty so they will be created with uninitialized ids (=0 because of calloc)\n");
	for (size_t i=1; i<=10; ++i) {
		mynode *node = mysll_pool_get(&pool);
		printnode(node, "");
		node->id = i;
		mysll_list_pushback(&list, node);
	}

	printf("-\niterating over the list, and removing items with id 1,4,5,10 and returning them to the pool\n");
	for (mysll_iter_start(&iter, &list); !mysll_iter_isend(&iter); mysll_iter_next(&iter)) {
		mynode *node = mysll_iter_get(&iter);
		// node id's are chosen to show that the iterator functions can handle
		// removing from arbitrary positions in the list
		if (node->id == 1 || node->id == 5 || node->id == 6 || node->id == 10) {
			printnode(node, " - this node will be removed from the list");
			mysll_iter_pop(&iter);
			mysll_pool_return(&pool, node);
		}
		else {
			printnode(node, "");
		}
	}
	printf("-\nprinting the list again\n");
	for (mysll_iter_start(&iter, &list); !mysll_iter_isend(&iter); mysll_iter_next(&iter)) {
		mynode *node = mysll_iter_get(&iter);
		printnode(node, "");
	}
	printf("-\nremoving and returning the first three list items to the pool\n");
	for (size_t i=0; i<3; ++i) {
		mynode *node = mysll_list_popfront(&list);
		printnode(node, "");
		mysll_pool_return(&pool, node);
	}

	printf("-\nprinting the list again\n");
	for (mysll_iter_start(&iter, &list); !mysll_iter_isend(&iter); mysll_iter_next(&iter)) {
		mynode *node = mysll_iter_get(&iter);
		printnode(node, "");
	}
	printf("-\ngetting items from the pool again and putting them at the end of the list, this time some will be recycled\n");
	for (size_t i=0; i<10; ++i) {
		mynode *node = mysll_pool_get(&pool);
		printnode(node, "");
		mysll_list_pushback(&list, node);
	}
	printf("-\nprinting the list again\n");
	for (mysll_iter_start(&iter, &list); !mysll_iter_isend(&iter); mysll_iter_next(&iter)) {
		mynode *node = mysll_iter_get(&iter);
		printnode(node, "");
	}
	mysll_pool_free_contents(&pool);
	mysll_list_free_contents(&list);
	return 0;
}
