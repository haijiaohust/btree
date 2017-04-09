#ifndef _BTREE_H__
#define _BTREE_H__

#define M_NUM 5

typedef int KeyType;

typedef struct Btreenode{
	char is_leaf;		//0 is leaf, 1 else;
	int keynum;
	KeyType key[M_NUM];
	struct Btreenode* parent;
	struct Btreenode* ptr[M_NUM + 1];
}Btreenode;

typedef struct s_result{
	Btreenode* ptr;
	int num;
}s_result;

typedef struct Btreeroot{
	Btreenode* node;
}Btreeroot;

Btreeroot* Btree_create();
Btreenode* Btree_create_node();
void Btree_destory(Btreenode*);
int Btree_search(Btreeroot*, KeyType);
int Btree_insert(Btreeroot*, KeyType);
void Btree_print(Btreenode*, int);

#endif