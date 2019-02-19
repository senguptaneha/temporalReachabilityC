#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>



int nVertices=1000;
int walkLength, numWalks, numStops; //numStops = numWalks * walkLength
int tp=0,fp=0,tn=0,fn=0;
int threshold = 0;
double t = 0.1;
int timestep = 0;
double stopsTime=0, statsTime =0, componentTime=0, queryTime = 0, querySystem=0, insertTime=0, deleteTime = 0, addTime = 0, removeTime = 0, buildStopsTime = 0;

double insertStdDev = 0, deleteStdDev = 0, queryStdDev = 0, addStdDev = 0, removeStdDev = 0;
int *srcStops, *dstStops;
int numUpdates = 0, numQueries = 0, numInserts = 0, numDeletes = 0, numAdds = 0, numRemoves = 0;
double tpDist=0, fnDist = 0;
double lazyProb = 0;
char type = 'A';
int numPos = 0, numNeg = 0;

struct edge{
	int vertexId;
};


struct node{
	int vertexId;
	struct edge *adj;	//out neighbors
	int *stops;
	int numNbrs;
	int sizeNbrs;
	int color;
	int pred;
};
struct node *graph;
struct node *graphR;

struct bfsNode{
	int vertexId;
	struct bfsNode *next;
} *queue = NULL, *rear = NULL;

void enqueue(int vertexId){
	struct bfsNode *b = (struct bfsNode*)malloc(sizeof(struct bfsNode));
	b->vertexId = vertexId;
	b->next = NULL;
	if (rear){
		rear->next = b;
		rear = rear->next;
	}
	if (queue == NULL){
		queue = b;
		rear = b;
	}
}

int dequeue(){
	if ((queue == NULL) && (rear == NULL)){
		return -1;	//empty queue
	}
	struct bfsNode *temp = queue;
	queue = queue->next;
	if (queue == NULL) rear = NULL;
	int v = temp->vertexId;
	free(temp);
	return v;
}

int *maxDegreeNodes(){
	int numNodesToMaintain = 10;
	int *maxSet = (int*)malloc(numNodesToMaintain*sizeof(int));
	int i = 0;
	for (i=0; i<numNodesToMaintain; i++) maxSet[i] = -1;

	for (i=0; i<nVertices;i++){
		int j = 0;
		while (j < numNodesToMaintain){
			if (maxSet[j] == -1){
				maxSet[j] = i;
				break;
			}
			if (graph[maxSet[j]].numNbrs < graph[i].numNbrs){
				int temp = i;
				while (j < numNodesToMaintain){
					int temp1 = maxSet[j];
					maxSet[j] = temp;
					temp = temp1;
					j++;
				}
				break;
			}
			j++;
		}
	}
	return maxSet;
}

int bfsDia(int s){
	int i,j;
	enqueue(s);
	for (i = 0; i < nVertices; i++){
		graph[i].color = 0;
	}
	graph[s].color = 1;
	int u = dequeue();
	int numNodes = 0;
	while (u>=0){
		numNodes += 1;
		for (j=0; j< graph[u].numNbrs; j++){
			int v = graph[u].adj[j].vertexId;
			if (graph[v].color == 0){
				graph[v].color = graph[u].color + 1;
				enqueue(v);
			}
		}
		u = dequeue();
	}
	int dia = 1;
	for (i=0;i<nVertices;i++)
		if ((graph[i].color - 1) > dia)
			dia = graph[i].color - 1;
	return dia;
}


void bfsSource(int s, int d){
	int i,j;
	for (i=0;i<nVertices;i++){
		graph[i].color = 0;
		graph[i].pred = -1;
		//if (graph[i].numNbrs > graph[s].numNbrs) s = i;
	}
	enqueue(s);
	graph[s].color = 1;
	int u = dequeue();
	int numNodes = 0;
	while (u>=0){
		numNodes += 1;
		for (j=0; j< graph[u].numNbrs; j++){
			int v = graph[u].adj[j].vertexId;
			if (graph[v].color == 0){
				graph[v].color = graph[u].color + 1;
				enqueue(v);
				graph[v].pred = u;
			}
			if (v == d) break;
		}
		u = dequeue();
	}
	//clear the queue
	while (dequeue() >= 0);

/*	if (graph[d].color > 0){
		int u = d;
		printf("Reverse Path:");
		while (u != s && u != -1){
			printf("%d ", u);
			u = graph[u].pred;
		}
		printf("%d\n",s);
	}
*/
}


int countEdges(){
	int numEdges = 0;
	int i;
	for (i=0;i<nVertices;i++)
		numEdges += graph[i].numNbrs;
	return numEdges;
}

/*Reads a directed graph, 1 edge per line, from filename into graph and its transpose in graphR*/
void readGraph(char* filename);

/*sample a neighbor of source node, from graph if dir = 0, from graphR otherwise*/
int sampleNeighbor(int sourceNode, int dir,unsigned int *seed);

/*return the set of stops after numWalks, each of walkLength*/
int randomWalk(int sourceNode, int dir, unsigned int *seed, int dest);

/*get the set of stops and sort it*/
int buildStops(int vertexId, int dir, unsigned int *seed, int dest);

/*returns 1 if a and b have an element in common, a and b are arrays of size numWalks*/
int doesIntersect(int *a, int *b, int numWalks, int debug);

/*returns 1 if u and v are strongly connected according to their set of stops*/
int isStronglyConnected(int u, int v);

void addNode(int u);

void removeNode(int u);

/*update graph and graphR with an edge between u and v*/
void insertEdge(int u, int v);

/*update graph and graphR by deleting edge between u and v*/
void deleteEdge(int u, int v);

/*Recompute set of stops and components*/
void recompute(int x, int y, int updateType);

/*updates tp, tn, fn, fp values*/
void processQuery(char *str);

int getTimestamp(char *str);

int main(int argc, char *argv[]){
	srand(time(NULL));
	if (argc < 9){
		printf("Usage:reachability graphFile numVertices streamFile queryFile c1 c2 lazyProb type[A/E]\n");
		exit(0);
	}
	char *filename = argv[1];
	struct timespec start, finish,s1,f1;
	double elapsed = 0, preProcessTime = 0;

	nVertices = atoi(argv[2]);
	char *streamFilename = argv[3];
	char *queryFilename = argv[4];
	double c1 = atof(argv[5]);
	int c2 = atoi(argv[6]);
	lazyProb = atof(argv[7]);
	type = argv[8][0];

	readGraph(filename);
	int k;
//	printf("Finished reading graph\n");fflush(stdout);
	clock_gettime(CLOCK_MONOTONIC,&start);
	int *maxSet = maxDegreeNodes();

	int d = 6,i;
	for (i = 0; i < 10;i++){
		int d1 = bfsDia(maxSet[i]);
		if (d1 > d) d = d1;
	}
	clock_gettime(CLOCK_MONOTONIC,&finish);

	printf("Initial : Estimated diamter = %d\n",d); fflush(stdout);
	preProcessTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	walkLength = c2*d;
	numStops = (int)(floor(c1*sqrt(nVertices*log(nVertices))));
	if (type == 'A')
		numWalks = numStops/walkLength;
	else
		numWalks = numStops;

	srcStops = (int*)malloc(sizeof(int)*(numStops+1));
	dstStops = (int*)malloc(sizeof(int)*(numStops+1));
//	printf("numWalks = %d, walkLength = %d\n",numWalks, walkLength);
//	printf("numEdges = %d\n",countEdges()); fflush(stdout);
	clock_gettime(CLOCK_MONOTONIC,&start);
	FILE *streamFile = fopen(streamFilename,"r");
	FILE *queryFile = fopen(queryFilename,"r");
	char streamStr[100], queryStr[100], streamTemp[100], queryTemp[100];

        fgets(streamStr,100,streamFile);
        fgets(queryStr,100,queryFile);

        strcpy(streamTemp,streamStr);
        strcpy(queryTemp,queryStr);
        int streamTimestamp = getTimestamp(streamTemp), queryTimestamp = getTimestamp(queryTemp);
        int startOfStream = streamTimestamp;
        char *str;
        int endOfStream=0, endOfQuery=0;
	int numEdges = 0;
	for (i = 0; i < nVertices; i++) numEdges += graph[i].numNbrs;

        while ((endOfQuery<1)||(endOfStream<1)){
                timestep++;
		//printf("Updates here = %d, streamTimestamp = %d, queryTimestamp = %d\n",numUpdates,streamTimestamp,queryTimestamp);fflush(stdout);
                if ((endOfStream==1)||((endOfQuery==0) && (queryTimestamp <= streamTimestamp))){
		//	printf("updatesHere = %d\n",numUpdates);
                        str = queryStr;
                        clock_gettime(CLOCK_MONOTONIC,&s1);
                        processQuery(str);
                  //      printf("%d,%d,%d,%d\n",tp,fp,tn,fn);
                    //    printf("query wait = %lf, update = %lf, numUpdates = %d, numQueries = %d\n",
                      //  (querySystem/numQueries),(updateTime/numUpdates), numUpdates, numQueries);
                        clock_gettime(CLOCK_MONOTONIC,&f1);
                        double timeNow = (f1.tv_sec - start.tv_sec) + (f1.tv_nsec - start.tv_nsec)/pow(10,9);
                        double arrivalTime = queryTimestamp - startOfStream;

                        //read the next query
                        if (fgets(queryStr,100,queryFile)==NULL){
                                endOfQuery = 1;
				streamTimestamp = queryTimestamp+1;
				continue;
                                //break;
                        }
                        strcpy(queryTemp,queryStr);
                        queryTimestamp = getTimestamp(queryTemp);

                }
                else if (endOfStream ==0){
                        str = streamStr;

                        numUpdates += 1;
                        char *tok = strtok(str," ");
                        tok = strtok(NULL," ");
                        int u = atoi(tok);
                        tok = strtok(NULL," ");
                        int v = atoi(tok);
                        if (str[0]=='I'){
	                        clock_gettime(CLOCK_MONOTONIC,&s1);
				insertEdge(u,v);
	                        clock_gettime(CLOCK_MONOTONIC,&f1);
				double oldMean = 0;
				if (numInserts > 0)
					oldMean = insertTime/numInserts;
				double x = (f1.tv_sec - s1.tv_sec) + (f1.tv_nsec - s1.tv_nsec)/pow(10,9);
				insertTime += x;
				numInserts += 1;
				double newMean = insertTime/numInserts;
				insertStdDev += (x - newMean)*(x - oldMean);
			}
			else if (str[0]=='D'){
	                        clock_gettime(CLOCK_MONOTONIC,&s1);
				deleteEdge(u,v);
	                        clock_gettime(CLOCK_MONOTONIC,&f1);
				double oldMean = 0;
				if (numDeletes > 0)
					oldMean = deleteTime/numDeletes;
				double x = (f1.tv_sec - s1.tv_sec) + (f1.tv_nsec - s1.tv_nsec)/pow(10,9);
				deleteTime += x;
				numDeletes += 1;
				double newMean = deleteTime/numDeletes;
				deleteStdDev = (x - newMean)*(x - oldMean);
			}
			else if (str[0]=='A'){
				clock_gettime(CLOCK_MONOTONIC, &s1);
				addNode(u);
				clock_gettime(CLOCK_MONOTONIC, &f1);
				addTime += (f1.tv_sec - s1.tv_sec) + (f1.tv_nsec - s1.tv_nsec)/pow(10,9);
				numAdds += 1;
			}
			else if (str[0]=='R'){
				clock_gettime(CLOCK_MONOTONIC, &s1);
				removeNode(u);
				clock_gettime(CLOCK_MONOTONIC, &f1);
				removeTime += (f1.tv_sec - s1.tv_sec) + (f1.tv_nsec - s1.tv_nsec)/pow(10,9);
				numRemoves += 1;
			}

                        if (fgets(streamStr,100,streamFile)==NULL){
                                endOfStream = 1;
                                continue;
                        }
                        strcpy(streamTemp,streamStr);
                        streamTimestamp = getTimestamp(streamTemp);
                }
        }

	clock_gettime(CLOCK_MONOTONIC,&finish);
	elapsed = finish.tv_sec - start.tv_sec + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	//printf("%d,%d,%d,%d\n",tp,fp,tn,fn);
	//printf("Processed Stream in %lf (stops=%lf, stats = %lf, component = %lf, queryTime= %lf, buildStopsTime = %lf,\
	//querySystem = %lf, insert = %lf, delete = %lf, preProcess = %lf) seconds\n",elapsed, stopsTime,statsTime,componentTime,(queryTime/numQueries),(buildStopsTime/numQueries),
	//querySystem,(insertTime/numInserts),(deleteTime/numDeletes), preProcessTime);
	size_t memSize = malloc_count_peak();
//        memSize /= (1024.0*1024.0);
	double acc = (1.0*(tp+tn))/(tp+fp+tn+fn);
	insertStdDev = sqrt(insertStdDev/(numInserts-1));
	deleteStdDev = sqrt(deleteStdDev/(numDeletes-1));
	queryStdDev = sqrt(queryStdDev/(numQueries-1));
//        printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",elapsed,(insertTime/numInserts),(deleteTime/numDeletes),(queryTime/numQueries),preProcessTime,memSize,acc);
//        printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%zu\t%lf\n",elapsed,(insertTime/numInserts),(deleteTime/numDeletes),(addTime/numAdds),(removeTime/numRemoves),
//		(queryTime/numQueries),preProcessTime,memSize,acc);
	printf("fn = %d, fp = %d\n", fn, fp);
        printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%zu\t%lf\n",1000*elapsed,(1000*insertTime/numInserts),(1000*deleteTime/numDeletes),(1000*addTime/numAdds),(1000*removeTime/numRemoves),
		(1000*queryTime/numQueries),1000*preProcessTime,memSize,acc);
//	printf("#%lf\t%lf\t%lf\n",insertStdDev,deleteStdDev, queryStdDev);
/*	d = 0;
	for (i = 0; i < 10;i++){
		int d1 = bfsDia();
		if (d1 > d) d = d1;
	}
	printf("Estimated Diameter here = %d\n",d);
	printf("NumEdges here = %d\n",countEdges());
	printf("Number of nodes added = %d\n",numAdds);
	printf("Number of nodes deleted = %d\n",numRemoves);*/
	fclose(streamFile);
	//printf("fnDist = %lf, tpDist = %lf\n",(fnDist/fn),(tpDist/tp));
	fclose(queryFile);
	printf("numPos = %d, numNeg = %d\n", numPos, numNeg);
}


void readGraph(char *filename){
	FILE *fp = fopen(filename,"r");
	graph = (struct node *)malloc(nVertices*sizeof(struct node));
	graphR = (struct node*)malloc(nVertices*sizeof(struct node));
	int i;
	for (i=0;i<nVertices;i++){
		graph[i].vertexId = i;
		graphR[i].vertexId = i;
		graph[i].adj = NULL;
		graphR[i].adj = NULL;
		graph[i].stops = NULL;
		graphR[i].stops = NULL;
		graph[i].numNbrs = 0;
		graphR[i].numNbrs = 0;
		graph[i].sizeNbrs = 0;
		graphR[i].sizeNbrs = 0;
	}

	char str[500];
	while (fgets(str,500,fp)){
		if (str[0]=='#') continue;
		char *tok = strtok(str,"\t");
		int u = atoi(tok);
		tok = strtok(NULL,"\t");
		int v = atoi(tok);
		if (u<=nVertices && v<=nVertices){
			graph[u].sizeNbrs += 1;
			graphR[v].sizeNbrs += 1;
		}
	}
	fclose(fp);
	for (i=0;i<nVertices;i++){
		graph[i].adj = (struct edge*)malloc(sizeof(struct edge)*graph[i].sizeNbrs);
		graphR[i].adj = (struct edge*)malloc(sizeof(struct edge)*graphR[i].sizeNbrs);
	}
	fp = fopen(filename,"r");
	while (fgets(str,500,fp)){
		if (str[0]=='#') continue;
		char *tok = strtok(str,"\t");
		int u = atoi(tok);
		tok = strtok(NULL,"\t");
		int v = atoi(tok);
		if (u<=nVertices && v<=nVertices){
			graph[u].adj[graph[u].numNbrs].vertexId = v;
			graph[u].numNbrs += 1;
			graphR[v].adj[graphR[v].numNbrs].vertexId = u;
			graphR[v].numNbrs += 1;
		}
	}
	fclose(fp);
}

int sampleNeighbor(int sourceNode, int dir, unsigned int *seed){
	double elapsed = 0,e=0;
	struct edge *x = graph[sourceNode].adj;
	int numNbrs = graph[sourceNode].numNbrs;
	if (dir){
		x = graphR[sourceNode].adj;
		numNbrs = graphR[sourceNode].numNbrs;
	}
	int sample = -1;
	if (numNbrs == 0) return sample;
	if (type=='E'){
		double isLazy = ((double) rand_r(seed))/RAND_MAX;
	        if (isLazy < lazyProb) return sourceNode;
	}
	int r = rand_r(seed)%numNbrs;
	//printf("r = %d,%d,%d\n",r,sourceNode,x[r].vertexId);
	sample = x[r].vertexId;
	return sample;
}

int randomWalk(int sourceNode, int dir, unsigned int *seed, int destId){
	int node = sourceNode, nStops = 0;
	int *stops = srcStops, i, j;
	if (dir==1) stops = dstStops;
	for (i=0;i<=numStops;i++) stops[i] = -1;	//Here is where we reset src/ dstStops to -1
	stops[nStops++] = node;
	for (i=0;i<numWalks;i++){
		node = sourceNode;
		for (j=0;j<walkLength;j++){
			int sample = sampleNeighbor(node,dir,seed);
			if (sample == destId) return 1;
			if (sample!=-1){
				node = sample;
				if (type == 'A')
					stops[nStops++] = sample;
			}
		}
		if (type == 'E')
			stops[nStops++] = node;
	}
	return 0;
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int buildStops(int vertexId, int dir, unsigned int * seed, int destId){
	struct timespec start, finish, mid;
	int i, *stops;

	clock_gettime(CLOCK_MONOTONIC, &start);
	int sawDest = randomWalk(vertexId,dir,seed, destId);
	if (sawDest == 1) return 1;
	clock_gettime(CLOCK_MONOTONIC, &mid);

	//sort the set of stops
	i = 0;
	if (dir==0)
		stops = srcStops;
	else 	stops = dstStops;
	while (i<=numStops && stops[i]>-1) i++;
	qsort(stops, i, sizeof(int), cmpfunc);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	double randomWalkTime = (mid.tv_sec - start.tv_sec) + (mid.tv_nsec - start.tv_nsec)/pow(10,9);
	double sortTime = (finish.tv_sec - mid.tv_sec) + (finish.tv_nsec -  mid.tv_nsec)/pow(10,9);
	//printf("Random walk time = %lf, sort time = %lf\n",randomWalkTime, sortTime);
	return 0;

}

int doesIntersect(int *a, int *b, int numStops, int debug){
	int nn = 0;
	if (debug){
		printf("A:");
		for (nn=0;nn<numStops;nn++){
			if (a[nn]==-1) break;
			if ((nn>0) && (a[nn] > a[nn-1])) printf(", %d. %d",nn,a[nn]);
		}
		printf("\n");
		printf("B:");
		for (nn=0;nn<numStops;nn++){
			if (b[nn]==-1) break;
			if ((nn>0) && (b[nn] > b[nn-1])) printf(", %d. %d",nn,b[nn]);
		}
		printf("\n");
	}
	int i=0,j=0;
	int answer = 0;
	while ((i<=numStops) && (j<=numStops)){
		if (debug)
			printf("i = %d, a[i] = %d, j = %d, b[j] = %d\n",i,a[i],j,b[j]);
		if ((a[i]==-1) || (b[j]==-1))
			break;

		if (a[i] == b[j]){
			answer = 1;
			break;
		}
		if (a[i] < b[j]) i++;
		else j++;
	}
//	free(a);
//	free(b);
	if (debug) printf("RETURNING %d\n",answer);
	return answer;
}

void addNode(int u){
	graph[u].numNbrs = 0;
	graphR[u].numNbrs = 0;
}

void removeNode(int u){
	int i;
	for (i=0; i < graph[u].numNbrs; i++){
		int v = graph[u].adj[i].vertexId;
		deleteEdge(u,v);
	}
	graph[u].numNbrs = 0;
	for (i=0; i < graphR[u].numNbrs; i++){
		int v = graphR[u].adj[i].vertexId;
		deleteEdge(v,u);
	}
	graphR[u].numNbrs = 0;
}

void insertEdge(int u, int v){
	//inserting the forward edge
	if (graph[u].numNbrs < graph[u].sizeNbrs){
		graph[u].adj[graph[u].numNbrs].vertexId = v;
		graph[u].numNbrs += 1;
	}
	else{
		int newSize = graph[u].sizeNbrs + 10;

		struct edge *newAdj = (struct edge *)malloc(sizeof(struct edge)*newSize);
		int i;
		for (i=0;i<graph[u].numNbrs;i++){
			newAdj[i].vertexId = graph[u].adj[i].vertexId;
		}
		free(graph[u].adj);
		graph[u].adj = newAdj;
		graph[u].sizeNbrs = newSize;
		graph[u].adj[graph[u].numNbrs].vertexId = v;
		graph[u].numNbrs += 1;
	}
	//inserting the backward edge
	if (graphR[v].numNbrs < graphR[v].sizeNbrs){
		graphR[v].adj[graphR[v].numNbrs].vertexId = u;
		graphR[v].numNbrs += 1;
	}
	else{
		int newSize = graphR[v].sizeNbrs + 10;
		struct edge *newAdj = (struct edge *)malloc(sizeof(struct edge)*newSize);

		int i;
		for (i=0;i<graphR[v].numNbrs;i++){
			newAdj[i].vertexId = graphR[v].adj[i].vertexId;
		}
		free(graphR[v].adj);
		graphR[v].adj = newAdj;
		graphR[v].sizeNbrs = newSize;
		graphR[v].adj[graphR[v].numNbrs].vertexId = u;
		graphR[v].numNbrs += 1;
	}

}

void deleteEdge(int u, int v){
	int indexOfNbr = 0,i;

	struct value * nodesToRebuild = NULL;
	//deleting the forward edge
	for (i=0; i< graph[u].numNbrs;i++){
		if (graph[u].adj[i].vertexId == v) break;
	}
	if (i>=graph[u].numNbrs) return;	//this edge not found
	indexOfNbr = i;
	int lastIndex = graph[u].numNbrs - 1;
	if (indexOfNbr < lastIndex){
		graph[u].adj[indexOfNbr].vertexId = graph[u].adj[lastIndex].vertexId;
	}
	graph[u].adj[lastIndex].vertexId = -1;
	graph[u].numNbrs -= 1;
	//deleting the backward edge
	for (i=0; i< graphR[v].numNbrs;i++){
		if (graphR[v].adj[i].vertexId == u) break;
	}
	if (i>=graphR[v].numNbrs) return;	//this edge not found
	indexOfNbr = i;
	lastIndex = graphR[v].numNbrs -1;
	if (indexOfNbr < lastIndex){
		graphR[v].adj[indexOfNbr].vertexId = graphR[v].adj[lastIndex].vertexId;
	}
	graphR[v].adj[lastIndex].vertexId = -1;
	graphR[v].numNbrs -= 1;
}



void processQuery(char *str){
	char *tok = strtok(str," ");
	tok = strtok(NULL," ");
	int source = atoi(tok);
	tok = strtok(NULL," ");
	int dest = atoi(tok);
	tok = strtok(NULL," ");
	int trueAns = atoi(tok);
	struct timespec start,finish;
	if (trueAns >= 1) numPos += 1;
	else numNeg += 1;
	unsigned int seed =  25231;
	int walkAns;
	clock_gettime(CLOCK_MONOTONIC,&start);

	int sawDest = buildStops(source, 0, &seed, dest);
	if (sawDest == 1) walkAns = 1;
	else{
		int sawSrc = buildStops(dest,1, &seed, source);
		if (sawSrc == 1) walkAns = 1;
		else{
			walkAns = doesIntersect(srcStops, dstStops, numStops,0);
		}
	}
	clock_gettime(CLOCK_MONOTONIC,&finish);

	double computeTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	double oldMean = 0;
	if (numQueries > 0)
		oldMean = queryTime/numQueries;
	queryTime += computeTime;
	numQueries += 1;
	double newMean = queryTime/numQueries;
	queryStdDev += (computeTime - oldMean)*(computeTime - newMean);

	if ((trueAns>0) && (walkAns>0)){
		tp++;
		tpDist += trueAns;
	}
	else if ((trueAns>0) && (walkAns==0)){
		fn++;
		fnDist += trueAns;
		//if (source!=dest) printf("trueAns = %d, source = %d, dest = %d\n",trueAns, source, dest);
//		bfsSource(source, dest);
		int i=0, prev = -1;
/*		printf("srcStops:");
		for (i=0;i<numStops;i++){
			if (srcStops[i] > prev) printf("%d, ",srcStops[i]);
			prev = srcStops[i];
		}
		printf("\n");
		i=0; prev = -1;
		printf("dstStops:");
		for (i=0;i<numStops;i++){
			if (dstStops[i] > prev) printf("%d, ", dstStops[i]);
			prev = dstStops[i];
		}
		printf("\n");*/
	}
	else if ((trueAns==0) && (walkAns>0)){
		fp++;
		printf("******************************\n");
		printf("On Query %d, %d [%s]\n",source, dest, str);


	}
	else if ((trueAns==0) && (walkAns==0)) tn++;
//	printf("[%d]Q: %d %d %d\n", numQueries, source, dest, walkAns);

}

int getTimestamp(char *str){
        char *tok = strtok(str," "); //A: or Q:
        if ((tok[0]=='I')||(tok[0]=='D')){
                tok = strtok(NULL," "); //source
                tok = strtok(NULL," "); //dest
                tok = strtok(NULL," "); //timestamp
                return (atoi(tok));
        }
        else if (tok[0]=='Q'){
                tok = strtok(NULL," "); //source
                tok = strtok(NULL," "); //dest
                tok = strtok(NULL," "); //true answer
                tok = strtok(NULL," "); //timestamp
                return (atoi(tok));
        }
	else if ((tok[0]=='A')||(tok[0]=='R')){
		tok = strtok(NULL," ");//vertex
		tok = strtok(NULL," ");	//timestamp
		return (atoi(tok));
	}
        return 0;
}

