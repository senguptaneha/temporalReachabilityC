#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include "TemporalGraph.h"

void TemporalGraph::addEdgeToIndex(int src, int dst, int startTime){
}

void TemporalGraph::removeEdgeFromIndex(int src, int dst, int startTime, int endTime){
}


int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	/*reset all Nodes*/
	bool isReached[numNodes];
	std::vector<int> queue;
	for (int i = 0; i < numNodes; i++) isReached[i] = false;
	int u = src;
	isReached[u] = true;
	queue.push_back(u);

	Interval *query = new Interval(startTime, endTime);
	while (queue.size() > 0){
		u = queue[0];
		queue.erase(queue.begin());

		//for each edge from u
		TemporalNode *n = nodes[u];
		for (int i = 0; i < n->getNumEdges(true); i++){
			TemporalEdge *e = n->getEdgeAt(i,true);
			int v = e->getDestId();
			Interval *x = new Interval(e->getStartTime(), e->getEndTime());
			if (x->isSubInterval(query) && isReached[v] == false){
				isReached[v] = true;
				queue.push_back(v);
			}
			free(x);
		}
	}
	free(query);

	if (isReached[dst] == true) return 1;
	return 0;
}
