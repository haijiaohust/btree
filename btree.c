#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG_ON

Btreeroot* Btree_create()
{
#ifdef DEBUG_ON
	printf("create tree begin\n");
#endif
	Btreeroot* root = (Btreeroot*)malloc(sizeof(Btreeroot));
	if(!root)
		return NULL;
	root->node = NULL;
#ifdef DEBUG_ON
	printf("create tree end %p\n", root);
#endif
	return root;
}
Btreenode* Btree_create_node()
{
	int i;
	Btreenode* p = (Btreenode*)malloc(sizeof(Btreenode));
	p->keynum = 0;
	for(i = 0; i < 2 * BTREE_T - 1; i++){
		p->key[i] = 0;
		p->ptr[i] = NULL;
	}
	p->ptr[2 * BTREE_T - 1] = NULL;
	p->is_leaf = 1;
	return p;
}
void Btree_destory(Btreenode* p)
{
	int i;
	if(!p)
		return;
	for(i = 0; i <= p->keynum; i++)
		Btree_destory(p->ptr[i]);
}
s_result Btree_search_node(Btreenode* node, KeyType key)
{
	Btreenode* p = node;
	int i = 0;
	if(!p)
		return (s_result){NULL, -1};
	while(i < p->keynum && p->key[i] < key)
		i++;
	if(i < p->keynum && p->key[i] == key)
		return (s_result){p, i};
	if(p->is_leaf)
		return (s_result){NULL, -1};
	return Btree_search_node(p->ptr[i], key);
}
int Btree_search(Btreeroot* root, KeyType key)
{
	if(!root)
		return -1;
	s_result res = {
		.ptr = NULL,
		.num = -1,
	};
	res = Btree_search_node(root->node, key);
	return res.ptr ? 0 : -1;
}
void Btree_insert_split(Btreenode* x, int index, Btreenode* y)
{
#ifdef DEBUG_ON
	printf("insert split\n");
#endif
	int i;
	Btreenode* z = Btree_create_node();
	z->is_leaf = y->is_leaf;
	z->keynum = BTREE_T - 1;
	for(i = 0; i < BTREE_T - 1; i++){
		z->key[i] = y->key[BTREE_T + i];
		y->key[BTREE_T + i] = 0;
	}
	if(!y->is_leaf)
		for(i = 0; i < BTREE_T; i++){
			z->ptr[i] = y->ptr[i + BTREE_T];
			y->ptr[i + BTREE_T] = NULL;
		}
	y->keynum = BTREE_T - 1;
	for(i = x->keynum; i > index; i--)
		x->ptr[i + 1] = x->ptr[i];
	x->ptr[i + 1] = z;
	for(i = x->keynum - 1; i >= index; i--)
		x->key[i + 1] = x->key[i];
	x->key[i + 1] = y->key[BTREE_T - 1];
	y->key[BTREE_T - 1] = 0;
	x->keynum++;
}
void Btree_insert_nonfull(Btreenode* x, KeyType key)
{
#ifdef DEBUG_ON
	printf("insert nonfull begin\n");
#endif
	int i = x->keynum - 1;
	if(x->is_leaf){
		while(i >= 0 && x->key[i] > key){
			x->key[i + 1] = x->key[i];
			i--;
		}
		x->key[i + 1] = key;
		x->keynum++;
	}
	else{
		while(i >= 0 && x->key[i] > key)
			i--;
		i++;
		if(x->ptr[i]->keynum == (2 * BTREE_T - 1)){
			Btree_insert_split(x, i, x->ptr[i]);
			if(key > x->key[i])
				i++;
		}
		Btree_insert_nonfull(x->ptr[i], key);
	}
}
int Btree_insert(Btreeroot* root, KeyType key)
{
#ifdef DEBUG_ON
	printf("insert begin\n");
#endif
	int i;
	if(Btree_search(root, key) == 0){
		printf("key = %d existed\n", key);
		return -1;
	}
	if(!root->node)
		root->node = Btree_create_node();
	Btreenode* p = root->node;
	Btreenode* node = NULL;
	if(p->keynum == (2 * BTREE_T - 1))
	{
		node = Btree_create_node();
		root->node = node;
		node->ptr[0] = p;
		node->is_leaf = 0;
		Btree_insert_split(node, 0, p);
		Btree_insert_nonfull(node, key);
	}
	else Btree_insert_nonfull(p, key);
	return 0;
}
void Btree_print(Btreenode* node, int layer)
{
	Btreenode* p = node;
	int i;
	if(p){
		printf("layer = %d keynum = %d is_leaf = %d\n", layer, p->keynum, p->is_leaf);
		for(i = 0; i < 2 * BTREE_T - 1; i++)
			printf("%c ", p->key[i]);
		printf("\n");
		printf("%p\n", p);
		for(i = 0; i <= 2 * BTREE_T; i++)
			printf("%p ", p->ptr[i]);
		printf("\n");
		layer++;
		for(i = 0; i <= p->keynum; i++)
			if(p->ptr[i])
				Btree_print(p->ptr[i], layer);
	}
	else printf("the tree is empty\n");
}
/*
static void _Btree_delete_key(s_result* res)
{
	if(!res->ptr){
		printf("delete node is NULL\n");
		return;
	}
	Btreenode* p = res->ptr;
	int num = res->num;
	int i;
	if(p->is_leaf){

	}
	else{	//p is leaf
		for(i = num; i < p->keynum - 1; i++)
			p->key[i] = p->key[i + 1];
		p->keynum--;
		if(p->keynum < (CEIL(M_NUM) - 1))
	}
}
int Btree_delete(Btreeroot* root, KeyType key)
{
	if(!root->node)
		return -1;
	s_result res = Btree_search_node(root->node, key);
	if(!res.ptr){
		printf("can not find key = %c", key);
		return -1;
	}
	_Btree_delete_key(&res);
	return 0;
}*/