#ifndef TEMPORALGRAPH_H
#define TEMPORALGRAPH_H

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

#include "TemporalNode.h"
#include "malloc_count-0.7/malloc_count.h"
#include "Stop.h"

class TemporalGraph{

public:
	TemporalGraph();
	TemporalGraph(char *graphfilename, char *streamfilename);
	void readGraph(char *graphfilename);
	void readStream(char *streamfilename);
	void addEdge(int src, int dst, int timestamp);
	void removeEdge(int src, int dst, int timestamp);

	void readAndRunQuery(char *queryfilename, int c = 1, bool fractional = false);
	int isReachable(int u, int v, int startTime, int endTime, int c = 1, bool fractional = false);

	int numberOfNodes();
	int numberOfEdges();
	int getMaxTimestamp();
	int diameter();

	/*For RandomReach*/
	int numWalks, numStops, walkLength;
	Stop *srcStops, *dstStops;
	int c_budget;
	double c_numWalks, c_walkLength;
	void initializeRRParams(int diameter);
	int buildStops(int sourceNode, bool direction, int startTime, int endTime, int dst, int c, bool fractional);
	bool rrParamsInitialized;
	double initializationTime;
	/*For measurement*/
	double queryTimeMean, queryTimeVar, insertTimeMean, insertTimeVar, deleteTimeMean, deleteTimeVar;
	double totalQueryTime, totalInsertTime, totalDeleteTime;
	int numInserts, numDeletes, numQueries;

	void changeForTopChain();

private:
	std::vector<TemporalNode*> nodes;
	int numNodes;
	int numEdges;
	int maxTimestamp;

	void addEdgeToIndex(int src, int dst, int startTime);
	void removeEdgeFromIndex(int src, int dst, int startTime, int endTime);



};

static int stopCompare(const void *s, const void *t){
    return ((Stop*) s)->getNodeId() - ((Stop*)t)->getNodeId();
}
#endif
