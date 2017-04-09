#include <stdio.h>
#include "btree.h"

#define LENGTH(a) (sizeof(a) / sizeof(a[0]))

int main()
{
	char s[] = "CNGAHEKQMFWLTZDPRXYS";
	int len = LENGTH(s);
	int i;
	Btreeroot* root = Btree_create();
	for(i = 0; i < len; i++){
		Btree_insert(root, s[i]);
		Btree_print(root->node, 1);
	}
	Btree_destory(root->node);
	return 0;
}