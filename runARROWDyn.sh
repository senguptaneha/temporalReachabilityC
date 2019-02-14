c_walklength=1
c_numwalks=1
datasetPath=Datafiles/RealData/enron/enron
n=87274
echo ${datasetPath}
#~/krongenAnalysis/Datasets/arrow ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_arrow.log
~/krongenAnalysis/Datasets/arrowBBFS ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_bbfs.log.1

datasetPath=Datafiles/RealData/epinions/epinions
n=131828
echo ${datasetPath}
#~/krongenAnalysis/Datasets/arrow ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_arrow.log
#~/krongenAnalysis/Datasets/arrowBBFS ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_bbfs.log.1

datasetPath=Datafiles/RealData/flickr-growth/flickr
n=2302936
echo ${datasetPath}
#~/krongenAnalysis/Datasets/arrow ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_arrow.log
~/krongenAnalysis/Datasets/arrowBBFS ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_bbfs.log.1

datasetPath=Datafiles/RealData/facebook/fb
n=63732
echo ${datasetPath}
#~/krongenAnalysis/Datasets/arrow ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_arrow.log
~/krongenAnalysis/Datasets/arrowBBFS ${datasetPath}Graph $n ${datasetPath}UpdateDyn ${datasetPath}QueryDyn ${c_walklength} ${c_numwalks} 0 A > ${datasetPath}_bbfs.log.1


