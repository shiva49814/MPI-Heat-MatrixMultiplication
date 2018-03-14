#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

for N in ${N};
do
	for PROC in ${PROCS};
	do 
		FILE=${RESULTDIR}/heat_strong_${N}_${PROC}
	    
		if [ ! -f ${FILE} ]
		then
		qsub -d $(pwd) -q mamba -l mem=120gb,procs=${PROC} -v SCHED=0,N=${N},PROC=${PROC} ./run_heat.sh
		fi
	done
done
