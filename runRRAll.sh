c_walklength=1
c_numwalks=3

c_budget=3
make clean
make bfsPersistent
for datasetPath in Datafiles/RealData/facebook/fb
do
	echo bfsPersistent ${datasetPath}
	./bfsPersistent  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_bfs2Persistent_log
done


#make clean
#make rrChained
#for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
#do
#	echo rrChained ${datasetPath}
#	./rrChained  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryC 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log
#done

#make clean
#make rrSnapshot
#datasetPath=Datafiles/RealData/facebook/fb
#fractional=1
#for c in 1 10 100
#do
#	echo rrSnapshot ${c} ${fractional}
#	./rrSnapshot  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query ${c} ${fractional} ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrSnapshot${c}${fractional}log
#done
