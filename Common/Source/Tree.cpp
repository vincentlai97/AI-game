#include "Tree.h"

Tree::Tree()
: root(NULL)
{
}

Tree::~Tree()
{
}

Node* Tree::GetNode(std::vector<unsigned> iters)
{
	Node* current = root;
	for (const unsigned iter : iters)
	{
		current = root->GetChildNode(iter);
	}
	return current;
}