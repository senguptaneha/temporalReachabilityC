datasetPath=Datafiles/RealData/enron/enron
c_budget=3
for c_numwalks in 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.1
do
	for c_walklength in 1 3 5
	do
		./rrPersistent ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryP 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log_${c_budget}_${c_numwalks}_${c_walklength}
	done
done
c_walklength=3
for c_numwalks in 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.1
do
	for c_budget in 1 3 5
	do
		./rrPersistent ${datasetPath}Graph ${datasetPath}Stream ${datasetPath}QueryP 1 1 ${c_walklength} ${c_numwalks} ${c_budget} > ${datasetPath}_rrChained_log_${c_budget}_${c_numwalks}_${c_walklength}
	done
done


