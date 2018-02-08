#include <iostream>
#include <stdlib.h>

#include "IntervalTree.h"
using namespace std;

int intersect(vector<Interval*> intervals, Interval *x){
	int result = 0;
	for (int i = 0; i < intervals.size(); i++)
		if (intervals[i]->doesOverlap(x)) result += 1;
	return result;
}

int main(int argc, char *argv[]){
	IntervalTree *t = new IntervalTree();
	vector<Interval*> intervals;
	vector<int> nodeIds;
	srand(time(NULL));
	int numT = atoi(argv[1]);
	int T = 100;
	for (int i = 0; i < numT; i++){
		int start = rand() % T;
		int end = rand() % T;
		if (start > end){
			int temp = start;
			start = end;
			end = temp;
		}
		if (rand () % 10 == 0) end = -1;
		Interval *interval = new Interval(start, end);
		intervals.push_back(new Interval(start,end));
		int u = rand() % 100;
		nodeIds.push_back(u);
//		cout << i << ": " << "Created Interval ( " << start << ", " << end << ", " << u << ")" << endl << flush;
		t->add(u, interval);
//		cout << "************TREE START*****************" << endl;
//		t->printTree();
//		cout << "***********TREE END********************" << endl;
	}


	for (int i = 0; i < numT; i++){
		Interval *x = intervals[i];
		vector<IntervalNode*> foundIntervals;
		t->search(x, foundIntervals);
		vector<Interval*> actualIntervals;
		vector<int> actualNodeIds;
		for (int j = 0; j < numT; j++)
			if (intervals[j]->doesOverlap(x)){
				actualIntervals.push_back(intervals[j]);
				actualNodeIds.push_back(nodeIds[j]);
			}
		for (int j = 0; j < actualIntervals.size(); j++){
			Interval *a = actualIntervals[j];
			bool flag = false;
			for (int k = 0; k < foundIntervals.size(); k++){
				Interval *f = foundIntervals[k]->interval;
				if ((f->getStartTime() == a->getStartTime()) && (f->getEndTime() == a->getEndTime())){
					flag = true;
					break;
				}
			}
			if (flag == false){
				cout << "Did not find interval (" << a->getStartTime() << ", " << a->getEndTime() << "," << actualNodeIds[j] << ")";
				cout << " for (" << x->getStartTime() << ", " << x->getEndTime() << ")" << endl;
			}
		}

		int a = foundIntervals.size();
		int b = intersect(intervals,x);
		int c = actualIntervals.size();
		if ((a != b) || (b != c) || (a!=c))
			cout << a << ", " << b << ", " << c << endl;
	}
	cout << "main: CP5 " << endl << flush;

	for (int i = 0; i< numT; i++){
		//Interval *x = intervals.back();
		//intervals.pop_back();
		Interval *x = intervals[i];
		IntervalNode *n = t->findInterval(t->root,nodeIds[i],x);
		t->removeNode(n);
	}
}
