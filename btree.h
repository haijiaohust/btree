#ifndef _BTREE_H__
#define _BTREE_H__

#define BTREE_T 73

typedef int KeyType;

typedef struct Btreenode{
	char is_leaf;		//1 is leaf, 0 else;
	int keynum;
	KeyType key[2 * BTREE_T - 1];
	struct Btreenode* ptr[2 * BTREE_T];
}Btreenode;

typedef struct s_result{
	Btreenode* ptr;
	int num;
}s_result;

typedef struct Btreeroot{
	Btreenode* node;
}Btreeroot;

extern unsigned int write_num;

Btreeroot* Btree_create();
Btreenode* Btree_create_node();
void Btree_destory(Btreenode*);
int Btree_search(Btreeroot*, KeyType);
int Btree_insert(Btreeroot*, KeyType);
void Btree_print(Btreenode*, int);
s_result Btree_search_node(Btreenode*, KeyType);
int Btree_delete(Btreeroot*, KeyType);

#endif