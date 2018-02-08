#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>

#include "TemporalGraph.h"
//#include "IntervalNode.h"

void addInterval(std::vector<Interval*> &bfsIntervals, int startTime, int endTime){
	Interval dummy;
	for (int i = 0; i < bfsIntervals.size(); i++){
		if (startTime <= bfsIntervals[i]->getEndTime()){
			int minStartTime = dummy.min(startTime, bfsIntervals[i]->getStartTime());
			int j = i + 1;
			while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, bfsIntervals[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(bfsIntervals[k]);
			bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
			bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
		if ((startTime <= bfsIntervals[i]->getStartTime()) && (endTime >= bfsIntervals[i]->getStartTime())){
			int minStartTime = startTime;
			int j = i + 1;
			while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, bfsIntervals[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(bfsIntervals[k]);
			bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
			bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
	}
	bfsIntervals.push_back(new Interval(startTime, endTime));

}

void TemporalGraph::addEdgeToIndex(int src, int dst, int startTime){

}

void TemporalGraph::removeEdgeFromIndex(int src, int dst, int startTime, int endTime){
}

int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	/*reset all Nodes*/
	bool isInQueue[numNodes];
	std::vector<Interval*> bfsIntervals[numNodes];
	std::vector<int> queue;
	for (int i = 0; i < numNodes; i++) isInQueue[i] = false;
	int u = src;
	addInterval(bfsIntervals[u], startTime, endTime);
	isInQueue[u] = true;
	queue.push_back(u);
	Interval *queryInterval = new Interval(startTime, endTime);
//	std::cout << "isReachable CP1 " << std::endl << std::flush;
	while (queue.size() > 0){
		u = queue[0];
		queue.erase(queue.begin());
		isInQueue[u] = false;

		//for each edge from u
		TemporalNode *n = nodes[u];
//		std::cout << "isReachable CP A1 " << std::endl << std::flush;

		std::vector<IntervalNode*> foundIntervals = n->getOverlappingEdges(queryInterval,true);
//		std::cout << "isReachable CP A2 " << std::endl << std::flush;
		for (int i = 0; i < foundIntervals.size(); i++){
			IntervalNode *e = foundIntervals[i];
			int v = e->destId;
			Interval *x = e->interval;
			for (int j = 0; j < bfsIntervals[u].size(); j++){
				Interval *z = x->intersection(bfsIntervals[u][j]);
				if (z == NULL) continue;
				bool subFlag = false;
				for (int k = 0; k < bfsIntervals[v].size(); k++){
					Interval *w = bfsIntervals[v][k];
					bool isSub = w->isSubInterval(z);
					if (isSub) subFlag = true;
				}
				if (!subFlag){
					addInterval(bfsIntervals[v], z->getStartTime(), z->getEndTime());
					if (isInQueue[v] == false){
						queue.push_back(v);
						isInQueue[v] = true;
					}
				}
				free(z);

			}
			//free(x);
		}
	}
//	std::cout << "isReachable CP2 " << std::endl << std::flush;

	if (bfsIntervals[dst].size() > 0) return 1;
	return 0;
}
