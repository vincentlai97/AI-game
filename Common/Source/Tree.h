#ifndef TREE_H
#define TREE_H

#include "Node.h"

typedef class t;

class Tree
{
public:
	Tree();
	~Tree();

	Node* GetNode(std::vector<unsigned> iters);

private:
	Node* root;

};

#endif