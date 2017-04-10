#include <stdio.h>
#include "btree.h"

#define LENGTH(a) (sizeof(a) / sizeof(a[0]))

int main()
{
	char s[] = "CNGAHEKQMFWLTZDPRXYSJOBU";
	int len = LENGTH(s);
	int i;
	s_result res;
	Btreeroot* root = Btree_create();
	for(i = 0; i < len - 1; i++){
		Btree_insert(root, s[i]);
	}
	printf("print begin:\n");
	Btree_print(root->node, 1);
	res = Btree_search_node(root->node, 'H');
	if(res.ptr)
		printf("%c %p\n", res.ptr->key[res.num], res.ptr);
	res = Btree_search_node(root->node, 'T');
	if(res.ptr)
		printf("%c %p\n", res.ptr->key[res.num], res.ptr);
	Btree_destory(root->node);
	return 0;
}