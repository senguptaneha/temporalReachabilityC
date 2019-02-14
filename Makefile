rrSnapshot : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o rrSnapshot TemporalMain.cpp rrSnapshot.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

rrPersistent : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o rrPersistent TemporalMain.cpp rrPersistent.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

rrChained : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o rrChained TemporalMain.cpp rrChained.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bfsPersistent : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bfsPersistent TemporalMain.cpp bfsPersistent1.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bfsChained : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bfsChained TemporalMain.cpp bfsChained.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bbfsChained : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bbfsChained TemporalMain.cpp bbfsChained.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bfsSnapshot : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bfsSnapshot TemporalMain.cpp bfsSnapshotC.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bbfsSnapshot : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bbfsSnapshot TemporalMain.cpp bbfsSnapshot.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bbfsPersistent : TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o
	       g++ -o bbfsPersistent TemporalMain.cpp bbfsPersistent.cpp TemporalGraph.o TemporalNode.o TemporalEdge.o Interval.o Stop.o malloc_count-0.7/malloc_count.o -lrt -ldl

bfsInterval : TemporalGraph.o TemporalNodeInterval.o TemporalEdge.o Interval.o IntervalNode.o IntervalTree.o
	       g++ -o bfsInterval TemporalMain.cpp IntervalSnapshot.cpp TemporalGraph.o TemporalNodeInterval.o TemporalEdge.o IntervalNode.o IntervalTree.o  Interval.o malloc_count-0.7/malloc_count.o -lrt -ldl

TemporalGraph.o : TemporalGraph.cpp TemporalGraph.h
		  g++ -c TemporalGraph.cpp -o TemporalGraph.o

TemporalNode.o : TemporalNode.cpp TemporalNode.h
		 g++ -c TemporalNode.cpp -o TemporalNode.o

TemporalNodeInterval.o : TemporalNodeInterval.cpp TemporalNode.h
		 g++ -c TemporalNodeInterval.cpp -o TemporalNodeInterval.o

TemporalEdge.o : TemporalEdge.cpp TemporalEdge.h
		 g++ -c TemporalEdge.cpp -o TemporalEdge.o

Interval.o : Interval.cpp Interval.h
	     g++ -c Interval.cpp -o Interval.o

Stop.o : Stop.cpp Stop.h
	 g++ -c Stop.cpp -o Stop.o

IntervalNode.o : IntervalNode.cpp IntervalNode.h
		 g++ -c IntervalNode.cpp -o IntervalNode.o

IntervalTree.o : IntervalTree.cpp IntervalTree.h
		 g++ -c IntervalTree.cpp -o IntervalTree.o

clean :
	rm -f rrSnapshot rrPersistent rrChained bfsPersistent bbfsPersistent bfsChained bbfsChained bfsSnapshot bbfsSnapshot bfsInterval Interval.o TemporalEdge.o TemporalNode.o TemporalGraph.o Stop.o
