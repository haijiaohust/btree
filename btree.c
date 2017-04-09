#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

Btreeroot* Btree_create()
{
	Btreeroot* root = (Btreeroot*)malloc(sizeof(Btreeroot));
	if(!root)
		return NULL;
	root->node = NULL;
	return root;
}
Btreenode* Btree_create_node()
{
	int i;
	Btreenode* p = (Btreenode*)malloc(sizeof(Btreenode));
	p->keynum = 0;
	p->parent = NULL;
	for(i = 0; i < M_NUM - 1; i++){
		p->key[i] = 0;
		p->ptr[i] = NULL;
	}
	p->ptr[M_NUM - 1] = NULL;
	p->is_leaf = 0;
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
s_result _Btree_search(Btreenode* node, KeyType key)
{
	Btreenode* p = node;
	int i = 0;
	while(i < p->keynum && p->key[i] < key)
		i++;
	if(p->key[i] == key)
		return (s_result){p, i};
	if(!p->is_leaf)
		return (s_result){NULL, -1};
	return _Btree_search(p->ptr[i], key);
}
int Btree_search(Btreeroot* root, KeyType key)
{
	if(!root)
		return -1;
	s_result res = {
		.ptr = NULL,
		.num = -1,
	};
	res = _Btree_search(root->node, key);
	return res.ptr ? 0 : -1;
}
Btreenode* _find_insert_pos(Btreeroot* root, KeyType key)
{
	int i;
	if(!root)
		return NULL;
	Btreenode* p = root->node;
	if(!p)
		return NULL;
	while(p->is_leaf){
		i = 0;
		while(i < p->keynum && p->key[i] < key)
			i++;
		p = p->ptr[i];
	}
	return p;
}
void _Btree_insert_split(Btreeroot* root, Btreenode* node)
{
	int i;
	int pos;
	Btreenode* p = NULL;
	Btreenode* q = NULL;
	if(node->is_leaf){
		p = Btree_create_node();
		pos = (M_NUM) / 2;
		p->keynum = node->keynum - pos - 1;
		for(i = pos + 1; i < node->keynum; i++)
			p->key[i - (pos + 1)] = node->key[i];
		for(i = pos + 1; i <= node->keynum; i++)
			p->ptr[i - (pos + 1)] = node->ptr[i];
		node->keynum /= 2;
		if(node->parent){
			q = node->parent;
			i = node->keynum - 1;
			for(; q->key[i] > node->key[pos]; i--){
				q->key[i + 1] = q->key[i];
				q->ptr[i + 2] = q->ptr[i + 1];
			}
			q->key[i + 1] = node->key[pos];
			q->ptr[i + 2] = q->ptr[i + 1];
			q->ptr[i + 1] = p;
			q->keynum++;
			p->parent = q;
			if(q->keynum == M_NUM)
				_Btree_insert_split(root, q);
		}
		else{
			q = Btree_create_node();
			q->keynum = 1;
			q->key[0] = node->key[pos];
			q->is_leaf = 1;
			q->ptr[0] = node;
			q->ptr[1] = p;

			root->node = q;
			node->parent = q;
			p->parent = q;
		}
	}
	else{	//leaf node
		p = Btree_create_node();
		pos = (M_NUM) / 2;
		p->keynum = node->keynum - pos - 1;
		for(i = pos + 1; i < node->keynum; i++)
			p->key[i - (pos + 1)] = node->key[i];
		node->keynum /= 2;
		if(node->parent){
			q = node->parent;
			i = node->keynum - 1;
			for(; q->key[i] > node->key[pos]; i--){
				q->key[i + 1] = q->key[i];
				q->ptr[i + 2] = q->ptr[i + 1];
			}
			q->key[i + 1] = node->key[pos];
			q->ptr[i + 2] = q->ptr[i + 1];
			q->ptr[i + 1] = p;
			q->keynum++;
			p->parent = q;
			if(q->keynum == M_NUM)
				_Btree_insert_split(root, q);
		}
		else{
			q = Btree_create_node();
			q->keynum = 1;
			q->key[0] = node->key[pos];
			q->is_leaf = 1;
			q->ptr[0] = node;
			q->ptr[1] = p;

			root->node = q;
			node->parent = q;
			p->parent = q;
		}
	}
}
int Btree_insert(Btreeroot* root, KeyType key)
{
	int i;
	if(Btree_search(root, key) == 0){
		printf("key = %d existed\n", key);
		return -1;
	}
	Btreenode* p = root->node;
	Btreenode* node = NULL;
	if(!p){
		node = Btree_create_node();
		node->keynum = 1;
		node->key[0] = key;
		return 0;
	}
	else{
		node = _find_insert_pos(root, key);
		if(!node){
			printf("find insert pos error\n");
			return -1;
		}
		i = node->keynum - 1;
		while(node->key[i] > key && i >= 0){
			node->key[i + 1] = node->key[i];
			i--;
		}
		node->key[i + 1] = key;
		node->keynum++;
		if(node->keynum == M_NUM)
			_Btree_insert_split(root, node);
		return 0;
	}
}
void Btree_print(Btreenode* node, int layer)
{
	Btreenode* p = node;
	int i;
	if(p){
		printf("\nlayer = %d keynum = %d is_leaf = %d\n", layer, p->keynum, p->is_leaf);
		for(i = 0; i < p->keynum; i++)
			printf("%d ", p->key[i]);
		printf("\n");
		printf("%p ", p->parent);
		for(i = 0; i <= p->keynum; i++)
			printf("%p ", p->ptr[i]);
		printf("\n");
		layer++;
		for(i = 0; i <= p->keynum; i++)
			if(p->ptr[i])
				Btree_print(p->ptr[i], layer);
	}
	else printf("the tree is empty\n");
}