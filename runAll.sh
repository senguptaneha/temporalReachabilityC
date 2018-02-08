for i in `seq 0 4`; do
	./TemporalMainRRSnapshot 262144 Datafiles/graphFile_9_0.5 Datafiles/graphFile_9_0.5_updateG Datafiles/graphFile_9_0.5_query${i} > logS_9_0.5_${i}
	./TemporalMainRRPersistent 262144 Datafiles/graphFile_9_0.5 Datafiles/graphFile_9_0.5_updateG Datafiles/graphFile_9_0.5_query${i} > logP_9_0.5_${i}
	./TemporalMainRRChained 262144 Datafiles/graphFile_9_0.5 Datafiles/graphFile_9_0.5_updateG Datafiles/graphFile_9_0.5_query${i} > logC_9_0.5_${i}
	./TemporalMainRRSnapshot 262144 Datafiles/graphFile_9_0.6 Datafiles/graphFile_9_0.6_updateG Datafiles/graphFile_9_0.6_query${i} > logS_9_0.6_${i}
	./TemporalMainRRPersistent 262144 Datafiles/graphFile_9_0.6 Datafiles/graphFile_9_0.6_updateG Datafiles/graphFile_9_0.6_query${i} > logP_9_0.6_${i}
	./TemporalMainRRChained 262144 Datafiles/graphFile_9_0.6 Datafiles/graphFile_9_0.6_updateG Datafiles/graphFile_9_0.6_query${i} > logC_9_0.6_${i}
	./TemporalMainRRSnapshot 1048576 Datafiles/graphFile_10_0.5 Datafiles/graphFile_10_0.5_updateG Datafiles/graphFile_10_0.5_query${i} > logS_10_0.5_${i}
	./TemporalMainRRPersistent 1048576 Datafiles/graphFile_10_0.5 Datafiles/graphFile_10_0.5_updateG Datafiles/graphFile_10_0.5_query${i} > logP_10_0.5_${i}
	./TemporalMainRRChained 1048576 Datafiles/graphFile_10_0.5 Datafiles/graphFile_10_0.5_updateG Datafiles/graphFile_10_0.5_query${i} > logC_10_0.5_${i}
done

