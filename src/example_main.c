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
	if (n == NULL) {
		printf("NULL node\n");
		return;
	}
	if (n->id != 0) {
		printf("node with id %d%s\n", n->id, note);
	}
	else {
		printf("uninitialized node%s\n", note);
	}
}

int main(void) {

	mypool pool = {0}; // important!, or use mysll_pclear()
	mylist list = {0}; // important!, or use mysll_lclear()

	printf("-\ngetting items from the pool, pool is empty so they will be created with uninitialized ids (=0 because of calloc)\n");
	for (size_t i=1; i<=10; ++i) {
		mynode *node = mysll_pget(&pool);
		printnode(node, "");
		node->id = i;
		mysll_lpushback(&list, node);
	}

	printf("-\niterating over the list, and removing items with id 1,4,5,10 and returning them to the pool\n");
	for (myiter iter=SLL_ISTART(&list); !mysll_iisend(&iter); mysll_inext(&iter)) {
		mynode *node = mysll_iget(&iter);
		// node id's are chosen to show that the iterator functions can handle
		// removing from arbitrary positions in the list
		if (node->id == 1 || node->id == 5 || node->id == 6 || node->id == 10) {
			printnode(node, " - this node will be removed from the list");
		//	printnode(node->sll_link_next, " next");
			mysll_ipop(&iter);
			mysll_preturn(&pool, node);
		}
		else {
			printnode(node, "");
		}
	}
	printf("-\nprinting the list again\n");
	for (myiter iter=SLL_ISTART(&list); !mysll_iisend(&iter); mysll_inext(&iter)) {
		mynode *node = mysll_iget(&iter);
		printnode(node, "");
	}
	printf("-\nremoving and returning the first three list items to the pool\n");
	for (size_t i=0; i<3; ++i) {
		mynode *node = mysll_lpopfront(&list);
		printnode(node, "");
		mysll_preturn(&pool, node);
	}

	printf("-\nprinting the list again\n");
	for (myiter iter=SLL_ISTART(&list); !mysll_iisend(&iter); mysll_inext(&iter)) {
		mynode *node = mysll_iget(&iter);
		printnode(node, "");
	}
	printf("-\ngetting items from the pool again and putting them at the end of the list, this time some will be recycled\n");
	for (size_t i=0; i<10; ++i) {
		mynode *node = mysll_pget(&pool);
		printnode(node, "");
		mysll_lpushback(&list, node);
	}
	printf("-\nprinting the list again\n");
	for (myiter iter=SLL_ISTART(&list); !mysll_iisend(&iter); mysll_inext(&iter)) {
		mynode *node = mysll_iget(&iter);
		printnode(node, "");
	}
	mysll_pfree(&pool);
	mysll_lfree(&list);
	return 0;
}
