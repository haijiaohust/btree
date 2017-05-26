#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG_ON
#define DEBUG_ON_RW
unsigned int write_num = 0;
#ifdef DEBUG_ON_RW
void disk_read(Btreenode* x)
{
	//printf("disk read\n");
}
void disk_write(Btreenode* x)
{
	write_num++;
	//printf("disk write\n");
}
#endif

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
#ifdef DEBUG_ON_RW
	disk_read(p->ptr[i]);
#endif
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
#ifdef DEBUG_ON_RW
	disk_write(x);
	disk_write(y);
	disk_write(z);
#endif
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
#ifdef DEBUG_ON_RW
		disk_write(x);
#endif
	}
	else{
		while(i >= 0 && x->key[i] > key)
			i--;
		i++;
#ifdef DEBUG_ON_RW
		disk_read(x->ptr[i]);
#endif
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
void Btree_merge(Btreeroot* root, Btreenode* node, int index)
{
	int i;
	Btreenode* left = node->ptr[index];
	Btreenode* right = node->ptr[index + 1];
	KeyType key = node->key[index];

	left->key[left->keynum] = key;
	left->ptr[left->keynum + 1] = right->ptr[0];
	left->keynum++;

	for(i = 0; i < right->keynum; i++){
		left->key[left->keynum] = right->key[i];
		left->ptr[left->keynum + 1] = right->ptr[i + 1];
		left->keynum++;
	}

	for(i = index; i < node->keynum - 1; i++){
		node->key[i] = node->key[i + 1];
		node->ptr[i + 1] = node->ptr[i + 2];
	}
	node->key[node->keynum - 1] = 0;
	node->ptr[node->keynum] = NULL;
	node->keynum--;

	if(node->keynum == 0){
		root->node = left;
		free(node);
	}
	free(right);
}
static void Btree_delete_key(Btreeroot* root, Btreenode* p, KeyType key)
{
	int i, index;
	Btreenode* node = p;
	Btreenode* tmp = NULL;
	KeyType tmpkey;
	if(!node)
		return;
	index = 0;
	while(index < node->keynum && key > node->key[index])
		index++;
	if(index < node->keynum && key == node->key[index])
	{
		//(1) 关键字key在叶子节点，直接删除
		if(node->is_leaf){
			for(i = index; i < node->keynum - 1; i++)
				node->key[i] = node->key[i + 1];
			node->key[node->keynum - 1] = 0;
			node->keynum--;
			if(node->keynum == 0){
				free(node);
				root->node = NULL;
			}
			return;
		}
		//(2a) node是内节点，左孩子节点至少t个关键字
		//左孩子的最后一个节点上移至key的位置
		//删除左孩子的最后一个节点
		else if(node->ptr[index]->keynum >= BTREE_T){
			tmp = node->ptr[index];
			tmpkey = tmp->key[tmp->keynum - 1];
			node->key[index] = tmpkey;
			Btree_delete_key(root, tmp, tmpkey);
		}
		//(2b) node是内节点，右孩子节点至少t个关键字
		//右孩子的第一个节点上移至key的位置
		//删除右孩子的第一个节点
		else if(node->ptr[index + 1]->keynum >= BTREE_T){
			tmp = node->ptr[index + 1];
			tmpkey = node->ptr[index + 1]->key[0];
			node->key[index] = tmpkey;
			Btree_delete_key(root, tmp, tmpkey);
		}
		//(2c) node是内节点，左右孩子节点都t - 1个关键字
		//将key的左右孩子和key合并
		//从合并后节点删除key
		else{
			Btree_merge(root, node, index);
			Btree_delete_key(root, node->ptr[index], key);
		}
	}
	else{
		Btreenode* left = NULL;
		Btreenode* right = NULL;
		Btreenode* child = node->ptr[index];
		if(!child){
			printf("Cannot del key = %d\n", key);
			return;
		}
		if(child->keynum == (BTREE_T - 1)){
			if(index - 1 >= 0)
				left = node->ptr[index - 1];
			if(index + 1 <= node->keynum)
				right = node->ptr[index + 1];
			//(3a)
			//相邻左右节点中有至少t个节点
			//将富裕的邻节点移动一个节点到该子节点
			if((left && left->keynum >= BTREE_T) ||
				(right && right->keynum >= BTREE_T))
			{
				int richR = 0;
				if(right)
					richR = 1;
				if(left && right)
					richR = (right->keynum > left->keynum) ? 1 : 0;
				if(right && right->keynum >= BTREE_T && richR){
					child->key[child->keynum] = node->key[index];
					child->ptr[child->keynum + 1] = right->ptr[0];
					child->keynum++;
					node->key[index] = right->key[0];
					for(i = 0; i < right->keynum - 1; i++){
						right->key[i] = right->key[i + 1];
						right->ptr[i] = right->ptr[i + 1];
					}
					right->key[right->keynum - 1] = 0;
					right->ptr[right->keynum - 1] = right->ptr[right->keynum];
					right->ptr[right->keynum] = NULL;
					right->keynum--;
				}
				else{
					for(i = child->keynum; i > 0; i--){
						child->key[i] = child->key[i - 1];
						child->ptr[i + 1] = child->ptr[i];
					}
					child->ptr[1] = child->ptr[0];
					child->ptr[0] = left->ptr[left->keynum];
					child->key[0] = node->key[index - 1];
					child->keynum++;

					left->key[left->keynum - 1] = 0;
					left->ptr[left->keynum]  = NULL;
					left->keynum--;
				}
			}
			//(3b)
			//相邻左右节点都只有t - 1个节点
			//合并节点
			else if((!left || (left && left->keynum == BTREE_T - 1))
				&& (!right || (right && right->keynum == BTREE_T - 1)))
			{
				if(left && left->keynum == BTREE_T - 1){
					Btree_merge(root, node, index - 1);
					child = left;
				}
				else if(right && right->keynum == BTREE_T - 1){
					Btree_merge(root, node, index);
				}
			}
		}
		Btree_delete_key(root, child, key);
	}
}
int Btree_delete(Btreeroot* root, KeyType key)
{
	if(!root->node)
		return -1;
	Btree_delete_key(root, root->node, key);
	return 0;
}