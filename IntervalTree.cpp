#include "IntervalTree.h"

using namespace std;


IntervalTree::IntervalTree(){
	sentinel = new IntervalNode();
	sentinel->parent = NULL;
	sentinel->left = sentinel->right = NULL;
	sentinel->color = 0;
	root = sentinel;
	root->left = root->right = root->parent = sentinel;
}

vector<Interval*> IntervalTree::search(int value){
	vector<Interval*> result;
	searchSubTree(root, value, result);
	return result;
}

void IntervalTree::search(Interval *i, vector<IntervalNode*> &result){
	searchSubTree(root, i, result);
}

Interval* IntervalTree::searchFirstOverlapping(Interval *i){
	IntervalNode *n = root;
	while ((n != sentinel) && (!n->interval->doesOverlap(i))){
		if ((n->left != sentinel) && (IntervalNode::compareTimes(n->left->maxEndTime, i->getStartTime()) >= 0))
			n = n->left;
		else
			n = n->right;
	}
	if (n == sentinel) return NULL;
	return n->interval;
}

void IntervalTree::searchSubTree(IntervalNode *node, Interval *i, vector<IntervalNode*> &result){
	if (node == sentinel) return;
//	cout << "Arrived at (" << node->interval->getStartTime() << ", " << node->interval->getEndTime() << ", " << node->destId << ", " << node->maxEndTime << ") "
//		<< "for (" << i->getStartTime() << ", " << i->getEndTime() << ")" << endl;
	if (node->left != sentinel) searchSubTree(node->left, i, result);
	if (i->doesOverlap(node->interval)) result.push_back(node);
	if ((node->right != sentinel) && (IntervalNode::compareTimes(i->getStartTime(), node->maxEndTime) <= 0)){
		searchSubTree(node->right, i, result);
	}
}

IntervalNode* IntervalTree::findInterval(IntervalNode *tree, int destId, Interval *i){
	while (tree != sentinel){
		int comp = tree->interval->compareTo(i);
		if ((comp == 0) && (destId == tree->destId)){
			return tree;
		}
		if (comp >= 0){
//		if ((comp > 0) || ((comp == 0) && (destId > tree->destId))){
			tree = tree->left;
			continue;
		}
		if (comp < 0){
//		if ((comp < 0) || ((comp == 0) && (destId < tree->destId))){
			tree = tree->right;
			continue;
		}
	}
	return sentinel;
}

void IntervalTree::searchSubTree(IntervalNode *node, int value, vector<Interval*> &result){
	if (node == sentinel) return;
	if (IntervalNode::compareTimes(value, node->maxEndTime) > 0) return;
	if (node->left != sentinel)
		searchSubTree(node->left, value, result);
	if (node->interval->contains(value)) result.push_back(node->interval);
	if (IntervalNode::compareTimes(value, node->interval->getStartTime()) < 0) return;
	if (node->right != sentinel)
		searchSubTree(node->right, value, result);
}
/**********************end of all search functions*************************/

void IntervalTree::add(int destId, Interval *interval){
	IntervalNode *n = new IntervalNode(destId, interval, sentinel);
	if (root == sentinel){
		n->color = 0;
		root = n;
	}
	else{
		free(n);
		insertInterval(destId, interval, root);
	}
}

void IntervalTree::insertInterval(int destId, Interval *interval, IntervalNode *currentNode){
	IntervalNode *addedNode = sentinel;
	int comp = interval->compareTo(currentNode->interval);
	if ((comp < 0) || ((comp == 0) && (destId !=  currentNode->destId))){
//	if (interval->compareTo(currentNode->interval) < 0){
		if (currentNode->left == sentinel){
			addedNode = new IntervalNode(destId, interval, sentinel);
			addedNode->color = 1;
			currentNode->left = addedNode;
			addedNode->parent = currentNode;
		}
		else{
			insertInterval(destId, interval, currentNode->left);
			return;
		}
	}
	else if (comp > 0){
//	if ((comp > 0) || ((comp == 0) && (destId > currentNode->destId))){
//	else if (interval->compareTo(currentNode->interval) > 0){
		if (currentNode->right == sentinel){
			addedNode = new IntervalNode(destId, interval, sentinel);
			addedNode->color = 1;
			currentNode->right = addedNode;
			addedNode->parent = currentNode;
		}
		else{
			insertInterval(destId, interval, currentNode->right);
			return;
		}
	}
	else return;
	/*std::cout << "insertInterval: Tree here " << std::endl;
	std::cout << "************************" << std:: endl;
	printTree();
	std::cout << "************************" << std:: endl;*/

	addedNode->parent->recalculateMaxEnd(sentinel);
	renewConstraintsAfterInsert(addedNode);
	root->color = 0;
}

void IntervalTree::renewConstraintsAfterInsert(IntervalNode *node){
	if (node->parent == sentinel) return;
	if (node->parent->color == 0) return;
	IntervalNode *uncle = node->uncle(sentinel);

	if ((uncle != sentinel) && (uncle->color == 1)){
		node->parent->color = uncle->color = 0;
		IntervalNode *gparent = node->grandParent(sentinel);
		if ((gparent != sentinel) && (!gparent->isRoot(sentinel))){
			gparent->color = 1;
			renewConstraintsAfterInsert(gparent);
		}
	}
	else{
		if ((node->parentDirection(sentinel) == 1) && (node->parent->parentDirection(sentinel) == -1)){
			rotateLeft(node->parent);
			node = node->left;
		}
		else if ((node->parentDirection(sentinel) == -1) && (node->parent->parentDirection(sentinel) == 1)){
			rotateRight(node->parent);
			node = node->right;
		}
		node->parent->color = 0;
		if (node->grandParent(sentinel) == sentinel) return;
		node->grandParent(sentinel)->color = 1;
		if (node->parentDirection(sentinel) == -1)
			rotateRight(node->grandParent(sentinel));
		else
			rotateLeft(node->grandParent(sentinel));
	}
}

void IntervalTree::removeNode(IntervalNode *node){
	if ((node == sentinel) || (node == NULL)) return;
	IntervalNode *temp = node;
	if ((node->right != sentinel) && (node->left != sentinel)){
		temp = node->successor(sentinel);
		node->interval = temp->interval;
		node->recalculateMaxEnd(sentinel);
		while (node->parent != sentinel){
			node = node->parent;
			node->recalculateMaxEnd(sentinel);
		}
	}
	node = temp;
	temp = (node->left != sentinel)? node->left : node->right;
	temp->parent = node->parent;
	if (node->isRoot(sentinel)) root = temp;
	else{
		if (node->parentDirection(sentinel) == -1)
			node->parent->left = temp;
		else
			node->parent->right = temp;
		IntervalNode *maxAux = node->parent;
		maxAux->recalculateMaxEnd(sentinel);
		while (maxAux->parent != sentinel){
			maxAux = maxAux->parent;
			maxAux->recalculateMaxEnd(sentinel);
		}
	}
	if (node->color == 0){
		renewConstraintsAfterDelete(temp);
	}

}

void IntervalTree::renewConstraintsAfterDelete(IntervalNode *node){
	while ((node != root) && (node->color == 0)){
		if (node->parentDirection(sentinel) == -1){
			IntervalNode *aux = node->parent->right;
			if (aux->color == 1){
				aux->color = 0;
				node->parent->color = 1;
				rotateLeft(node->parent);
				aux = node->parent->right;
			}

			if ((aux->left->color == 0) && (aux->right->color == 0)){
				aux->color = 1;
				node = node->parent;
			}
			else{
				if (aux->right->color == 0){
					aux->left->color = 0;
					aux->color = 1;
					rotateRight(aux);
					aux = node->parent->right;
				}
				aux->color = node->parent->color;
				node->parent->color = 0;
				aux->right->color = 0;
				rotateLeft(node->parent);
				node = root;
			}
		}
		else{
			IntervalNode *aux = node->parent->left;
			if (aux->color == 1){
				aux->color = 0;
				node->parent->color = 1;
				rotateRight(node->parent);
				aux = node->parent->left;
			}

			if ((aux->left->color == 0) && (aux->right->color == 0)){
				aux->color = 1;
				node = node->parent;
			}
			else{
				if (aux->left->color == 0){
					aux->right->color = 0;
					aux->color = 1;
					rotateLeft(aux);
					aux = node->parent->left;
				}
				aux->color = node->parent->color;
				node->parent->color = 0;
				aux->left->color = 0;
				rotateRight(node->parent);
				node = root;
			}
		}
	}
	node->color = 0;
}

void IntervalTree::rotateLeft(IntervalNode *node){
	IntervalNode *pivot = node->right;
	int dir = node->parentDirection(sentinel);
	IntervalNode *parent = node->parent;
	IntervalNode *tempTree = pivot->left;

	pivot->left = node;
	node->parent = pivot;
	node->right = tempTree;

	if (tempTree != sentinel) tempTree->parent = node;

	if (dir == 1) parent->right = pivot;
	else if (dir == -1) parent->left = pivot;
	else root = pivot;
	pivot->parent = parent;
	pivot->recalculateMaxEnd(sentinel);

	node->recalculateMaxEnd(sentinel);
}

void IntervalTree::rotateRight(IntervalNode *node){
	IntervalNode *pivot = node->left;
	int dir = node->parentDirection(sentinel);
	IntervalNode *parent = node->parent;
	IntervalNode *tempTree = pivot->right;

	pivot->right = node;
	node->parent = pivot;
	node->left = tempTree;

	if (tempTree != sentinel) tempTree->parent = node;
	if (dir == 1) parent->right = pivot;
	else if (dir == -1) parent->left = pivot;
	else root = pivot;

	pivot->parent = parent;
	pivot->recalculateMaxEnd(sentinel);
	node->recalculateMaxEnd(sentinel);
}

void IntervalTree::printTree(){
	root->printInterval();
}

