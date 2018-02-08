c_walklength=1
c_numwalks=1
c_budget=3
make clean
make rrPersistent
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo rrPersistent ${datasetPath}
#	./rrPersistent  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrPersistent_log
done


make clean
make rrChained
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo rrChained ${datasetPath}
	./rrChained  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryC 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log.1
done

make clean
make rrSnapshot
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo rrSnapshot ${datasetPath}
#	./rrSnapshot  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrSnapshot_log
done
