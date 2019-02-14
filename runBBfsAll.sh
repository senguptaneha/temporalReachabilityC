
#make clean
#make bbfsChained
#for datasetPath in Datafiles/RealData/enron/enron Datafiles/RealData/epinions/epinions Datafiles/RealData/flickr-growth/flickr
#do
#	echo bbfsChained ${datasetPath}
#	./bbfsChained  ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryC 1 1 > ${datasetPath}_bbfsChained_log
#done


#Usage: <TemporalReachability> graphFile updateFile queryFile [[c fractional] [c_walklength c_numWalks c_budget]]

make clean
make bbfsSnapshot
datasetPath=Datafiles/RealData/facebook/fb
fractional=1
for c in 1 10 100
do
	./bbfsSnapshot ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}Query $c $fractional > ${datasetPath}_bbfsSnapshot${c}${fractional}log
done

