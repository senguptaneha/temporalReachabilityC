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

int TemporalGraph::buildStops(int src, bool direction, int startTime, int endTime){
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();

	int stopIndex = 0;
	stops[stopIndex].setNodeId(src);

	for (int i = 0; i < numWalks; i++){
		int budget = 2 * walkLength;
		Interval *currInterval = new Interval(startTime, endTime);
		int currNode = src;
		bool noEdges = false;
		for (int j = 0; j < walkLength; j++){
			TemporalNode *n = nodes[currNode];
			TemporalEdge *e = NULL;
			bool foundEdge = false;
			while (budget > 0){
				budget -= 1;
				e = n->sampleEdge(direction);

				if (e == NULL){
					noEdges = true;
					break;
				}
				if ((e->getInterval())->isSubInterval(currInterval)){
					foundEdge = true;
					break;
				}
			}
			if (foundEdge){
				stopIndex += 1;
				if (stopIndex <= numStops){
					stops[stopIndex].setNodeId(e->getDestId());
				}
				currNode = e->getDestId();
			}
			if ((budget == 0) || (noEdges)) break;
		}

//		if (budget == 0) break;
	}
	return stopIndex;
}


int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	int numSrcStops = buildStops(src, true, startTime, endTime);
	int numDstStops = buildStops(dst, false, startTime, endTime);
	for (int i = 0; i < numSrcStops; i++){
		if (srcStops[i].getNodeId() == -1) continue;
		for (int j = 0; j < numDstStops; j++){
			if (dstStops[j].getNodeId() == -1) continue;
			if (srcStops[i].getNodeId() == dstStops[j].getNodeId()) return 1;
		}
	}
	return 0;
}
