#include "IntervalNode.h"

int IntervalNode::compareTimes(int t1, int t2){
	if ((t1 == -1) && (t2 >= 0)) return 1;
	if ((t2 == -1) && (t1 >= 0)) return -1;
	if ((t1 == -1) && (t2 == -1)) return 0;
	if (t1 > t2) return 1;
	if (t1 < t2) return -1;
	return 0;
}

IntervalNode::IntervalNode(){
	parent = left = right = NULL;
	maxEndTime = -1;
	interval = NULL;
	destId = -1;
	color = 0;
}

IntervalNode::IntervalNode(int isDestId, IntervalNode *sentinel){
	parent = left = right = sentinel;
	maxEndTime = -1;
	interval = NULL;
	destId = isDestId;
	color = 0;	//black
}


IntervalNode::IntervalNode(int isDestId, Interval *isInterval, IntervalNode *sentinel){
	interval = isInterval;
	maxEndTime = interval->getEndTime();
	destId = isDestId;
	parent = left = right = sentinel;
	color = 0;
}

bool IntervalNode::isRoot(IntervalNode *sentinel){
	return (parent == sentinel);
}

bool IntervalNode::isLeaf(IntervalNode *sentinel){
	return ((right == sentinel) && (left == sentinel));
}

/*0 if parent is null, -1 if I am left child, 1 otherwise*/
int IntervalNode::parentDirection(IntervalNode *sentinel){
	if (parent == sentinel) return 0;
	if (parent->left == this) return -1;
	return 1;
}

IntervalNode* IntervalNode::successor(IntervalNode *sentinel){
	if (right == sentinel) return sentinel;
	IntervalNode *n = right;
	while (n->left != sentinel) n = n->left;
	return n;
}

int IntervalNode::compareTo(IntervalNode *other, IntervalNode *sentinel){
	if ((other == NULL) || (other == sentinel)) return 5; //!
	int x = interval->compareTo(other->interval);
	if (x != 0) return x;
	if (destId < other->destId) return -1;
	if (destId > other->destId) return 1;
	return 0;
}

void IntervalNode::recalculateMaxEnd(IntervalNode *sentinel){
//	std::cout << "recalculateMaxEnd: CP1 (" << interval->getEndTime() << " , " << left->maxEndTime << ", " << right->maxEndTime << ")" << std::endl << std::flush;
	int max = interval->getEndTime();
	if (right != sentinel){
		if (compareTimes(right->maxEndTime, max) > 0)
			max = right->maxEndTime;
	}
//	std::cout << "recalculateMaxEnd: CP2 max = " << max << std::endl << std::flush;
	if (left != sentinel){
		if (compareTimes(left->maxEndTime, max) > 0)
			max = left->maxEndTime;
	}
//	std::cout << "recalculateMaxEnd: CP3 max = " << max << std::endl << std::flush;
	maxEndTime = max;
	if (parent != sentinel)
		parent->recalculateMaxEnd(sentinel);
}

IntervalNode* IntervalNode::grandParent(IntervalNode *sentinel){
	if (parent != sentinel) return parent->parent;
	return sentinel;
}

IntervalNode* IntervalNode::uncle(IntervalNode *sentinel){
	IntervalNode *gp = grandParent(sentinel);
	if (gp == sentinel) return sentinel;
	if (parent == gp->left) return gp->right;
	return gp->left;
}

IntervalNode* IntervalNode::sibling(IntervalNode *sentinel){
	if (parent != sentinel){
		if (parent->right == this) return parent->left;
		return parent->right;
	}
	return sentinel;
}

void IntervalNode::printInterval(){
	if (interval == NULL) return;	//sentinel
	std::cout << "( " << interval->getStartTime() << ", " << interval->getEndTime() << ", " << destId << ", " << maxEndTime << ")";
	std::cout << " P: ";
	if ((parent) && (parent->interval)) std::cout << "(" << parent->interval->getStartTime() << ", " << parent->interval->getEndTime() <<
	", " << parent->destId << ", " << parent->maxEndTime << ") ";
	std::cout << std::endl;
	std::cout << "L:" << std::endl;
	if (left) left->printInterval();
	std::cout << "R:" << std::endl;
	if (right) right->printInterval();
}
