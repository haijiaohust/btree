#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

#define LENGTH(a) (sizeof(a) / sizeof(a[0]))

int main()
{
	char s[] = "CNGAHEKQMFWLTZDPRXYSJOBUV";
	int len = LENGTH(s);
	int i;
	s_result res;

	Btreeroot* root = Btree_create();
	for(i = 0; i < len;){
		if(!Btree_insert(root, s[i]))
			i++;
	}
	printf("print begin:\n");
	Btree_print(root->node, 1);
	res = Btree_search_node(root->node, 'H');
	if(res.ptr)
		printf("%c %p\n", res.ptr->key[res.num], res.ptr);
	res = Btree_search_node(root->node, 'T');
	if(res.ptr)
		printf("%c %p\n", res.ptr->key[res.num], res.ptr);
	Btree_delete(root, 'U');
	printf("\nprint begin:\n");
	Btree_print(root->node, 1);
	Btree_destory(root);
	return 0;
}