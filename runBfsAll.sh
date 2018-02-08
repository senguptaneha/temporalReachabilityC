make clean
make bfsSnapshot
#for datasetPath in Datafiles/SyntheticData/EGS Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo bfsSnapshot ${datasetPath}
#	./bfsSnapshot  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query 1 1 > ${datasetPath}_bfsSnapshot_log
done

make clean
make bfsChained
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo bfsChained ${datasetPath}
	./bfsChained  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryC 1 1 > ${datasetPath}_bfsChained_log
done

make clean
make bfsPersistent
for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo bfsPersistent ${datasetPath}
#	./bfsPersistent  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query 1 1 > ${datasetPath}_bfsPersistent_log
done
