#ifndef _NUI_TREE_
#define _NUI_TREE_

#include <iterator>
#include <list>

template<typename TKey, typename TValue> class nuiTreeNode;
template<typename TKey, typename TValue> class nuiTree;
template<typename TKey, typename TValue> class nuiTreeDFSIterator;
template<typename TKey, typename TValue> class nuiTreeBFSIterator;

template<typename TKey, typename TValue>
class nuiTreeNode
{
public:
	nuiTreeNode(TKey key, TValue value)
	{
		this->key = key;
		this->value = value;
		this->parent = NULL;
		this->child = NULL;
		this->leftSibling = NULL;
		this->rightSibling = NULL;
	}
	~nuiTreeNode()
	{
		while (child!=NULL)
		{
			nuiTreeNode* tempNode = child->rightSibling;
			delete child;
			child = tempNode;
		}
		if (leftSibling!=NULL)
			leftSibling->rightSibling = rightSibling;
		if (rightSibling!=NULL)
			rightSibling->leftSibling = leftSibling;
		parent = NULL;
	}
public:
	void addChildNode(nuiTreeNode<TKey,TValue> *childNode)
	{
		if (childNode == NULL)
			return;
		childNode->parent = this;
		childNode->rightSibling = child;
		if (child!=NULL)
		{
			childNode->leftSibling = child->leftSibling;
			child->leftSibling = childNode;
		}
		else
		{
			childNode->leftSibling = NULL;
		}
		child = childNode;
	}
	void removeChildNode(TKey key)
	{
		if (key == NULL)
			return;
		nuiTreeNode* currentNode = child;
		while (currentNode!=NULL)
		{
			if (currentNode->key == key)
			{
				delete currentNode;
				break;
			}
			currentNode = currentNode->rightSibling;
		}
	}
	nuiTreeNode<TKey,TValue> *getChild(TKey key)
	{
		nuiTreeNode<TKey,TValue>* currentNode = child;
		while (currentNode!=NULL)
		{
			if (currentNode->key == key)
				return currentNode;
			currentNode = currentNode->rightSibling;
		}
		return NULL;
	}
	nuiTreeNode<TKey,TValue> *getChild()
	{
		return child;
	}
	nuiTreeNode<TKey,TValue> *getParent()
	{
		return parent;
	}
	nuiTreeNode<TKey,TValue> *getRightSibling()
	{
		return rightSibling;
	}
	nuiTreeNode<TKey,TValue> *getLeftSibling()
	{
		return leftSibling;
	}
public:
	TKey getKey() { return key;}
	TValue getValue() { return value;}
private:
	nuiTreeNode<TKey,TValue>* parent;
	nuiTreeNode<TKey,TValue>* child;
	nuiTreeNode<TKey,TValue>* rightSibling;
	nuiTreeNode<TKey,TValue>* leftSibling;
	TKey key;
	TValue value;
public:
	friend class nuiTreeDFSIterator<TKey,TValue>;
};

template<typename TKey, typename TValue>
class nuiTreeDFSIterator
{
public:
	nuiTreeDFSIterator(nuiTreeNode<TKey,TValue>* node) : currentNode(node) 
	{
		printf("+");
	}
	~nuiTreeDFSIterator()
	{
		printf("-");
	}
	nuiTreeDFSIterator(const nuiTreeDFSIterator& mit) : currentNode(mit.currentNode) 
	{
	}
	nuiTreeDFSIterator& operator++() 
	{
		if (currentNode->child!=NULL)
		{
			currentNode = currentNode->child;
			return *this;
		}
		if (currentNode->rightSibling != NULL)
		{
			currentNode = currentNode->rightSibling;
			return *this;
		}
		currentNode = currentNode->parent;
		while ((currentNode!=NULL) && (currentNode->rightSibling == NULL))
			currentNode = currentNode->parent;
		if (currentNode!=NULL)
			currentNode = currentNode->rightSibling;
		return *this;
	}
	bool operator==(const nuiTreeDFSIterator<TKey,TValue>& rhs) {return currentNode==rhs.currentNode;}
	bool operator!=(const nuiTreeDFSIterator<TKey,TValue>& rhs) {return currentNode!=rhs.currentNode;}
	nuiTreeNode<TKey,TValue>* &operator*() {return currentNode;}
private:
	nuiTreeNode<TKey,TValue>* currentNode;
};

template<typename TKey, typename TValue>
class nuiTree
{
public:
	typedef typename nuiTreeDFSIterator<TKey,TValue> iterator;

public:
	nuiTree(nuiTreeNode<TKey,TValue>* root) : rootNode(root)
	{
		beginIterator = new nuiTreeDFSIterator<TKey,TValue>(rootNode);
		endIterator = new nuiTreeDFSIterator<TKey,TValue>(NULL);
	}
	nuiTreeDFSIterator<TKey,TValue> begin() const
	{
		return *beginIterator;
	}
	nuiTreeDFSIterator<TKey,TValue> end() const
	{
		return *endIterator;
	}
private:
	nuiTreeNode<TKey,TValue>* rootNode;
	nuiTreeDFSIterator<TKey,TValue>* beginIterator;
	nuiTreeDFSIterator<TKey,TValue>* endIterator;
};

#endif//_NUI_TREE_