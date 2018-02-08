for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
do
	echo bbfsChained ${datasetPath}
	./bbfsChained  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryC 1 1 > ${datasetPath}_bbfsChained_log
done

