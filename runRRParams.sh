datasetPath=Datafiles/RealData/enron/enron
c_budget=3
for c_numwalks in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0
do
	for c_walklength in 1 3 5
	do
		./rrPersistent ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryP 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log_${c_budget}_${c_numwalks}_${c_walklength}
	done
done
c_walklength=3
for c_numwalks in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0
do
	for c_budget in 1 3 5
	do
		./rrPersistent ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryP 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log_${c_budget}_${c_numwalks}_${c_walklength}
	done
done


