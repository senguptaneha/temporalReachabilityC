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
//last 3 params not actually used, only for compatibility across methods.
int TemporalGraph::buildStops(int src, bool direction, int startTime, int endTime, int dst, int c, bool fractional){
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();

	int stopIndex = 0;
//	int budget = 2 * numStops;
	stops[stopIndex].setNodeId(src);
	stops[stopIndex].setStartTime(startTime);
	stops[stopIndex].setEndTime(endTime);

	for (int i = 0; i < numWalks; i++){
		int budget = 2 * walkLength;
		int currNode = src;
		bool noEdges = false;

		int t = startTime;
		if (!direction) t = endTime;

		for (int j = 0; j < walkLength; j++){
			TemporalNode *n = nodes[currNode];
			TemporalEdge *e;
			bool foundEdge = false;
			while (budget > 0){
				budget -= 1;
				e = n->sampleEdge(direction);

				if (e == NULL){
					noEdges = true;
					break;
				}
				if (direction){
					if ((e->getStartTime() >= t) && (e->getEndTime() != -1)){
						foundEdge = true;
						break;
					}
				}
				else{
					if ((e->getEndTime() != -1) && (e->getEndTime() <= t)){
						foundEdge = true;
						break;
					}
				}
			}
			if (foundEdge){
				stopIndex += 1;
				if (stopIndex <= numStops){
					stops[stopIndex].setNodeId(e->getDestId());
					if (direction)
						stops[stopIndex].setStartTime(e->getEndTime());
					else
						stops[stopIndex].setStartTime(e->getStartTime());
				}
				currNode = e->getDestId();
				if (direction) t = e->getEndTime();
				else t = e->getStartTime();
			}
			if ((budget == 0) || (noEdges)) break;
		}

//		if (budget == 0) break;
	}
	return stopIndex;
}


int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	int numSrcStops = buildStops(src, true, startTime, endTime, dst, c, fractional);
	int numDstStops = buildStops(dst, false, startTime, endTime, src, c, fractional);
	qsort(srcStops, numSrcStops, sizeof(Stop), stopCompare);
	qsort(dstStops, numDstStops, sizeof(Stop), stopCompare);

    int srcIndex = 0, dstIndex = 0;
    while (srcIndex < numSrcStops && dstIndex < numDstStops){
        int sNode = srcStops[srcIndex].getNodeId(), dNode = dstStops[dstIndex].getNodeId();
        if (sNode == -1) srcIndex++;
        else if (dNode == -1) dstIndex++;
        else if (sNode < dNode) srcIndex++;
        else if (dNode < sNode) dstIndex++;
        else if (sNode == dNode){
            int endSIndex = srcIndex, endDIndex = dstIndex;
            while (endSIndex < numSrcStops && srcStops[endSIndex].getNodeId() == sNode) endSIndex++;
            while (endDIndex < numDstStops && dstStops[endDIndex].getNodeId() == dNode) endDIndex++;
            if (endSIndex == numSrcStops) endSIndex = numSrcStops - 1;
            if (endDIndex == numDstStops) endDIndex = numDstStops - 1;

            for (int sI = srcIndex; sI <= endSIndex; sI++){
                for (int dI = dstIndex; dI <= endDIndex; dI++){
					if (srcStops[sI].getStartTime() <= dstStops[dI].getStartTime()) return 1;
                }
            }


            srcIndex = endSIndex + 1;
            dstIndex = endDIndex + 1;
        }
	}

	return 0;
}
