  GNU nano 2.3.1                                                  File: queue_sequential_heat.sh                                                                                                            

#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


for N in ${N_MATMUL_STRONG};
do
  	for PROC in ${PROCS_MATMUL};
        do
          	FILE=${RESULTDIR}/matmul_0_${N}_${PROC}

                if [ ! -f ${FILE} ]
                then
                qsub -d $(pwd) -q mamba -l mem=120gb,procs=${PROC} -v SCHED=0,N=${N},PROC=${PROC} ./run_matmul.sh
                fi
        done
done
 

for N in ${N_MATMUL_WEAK};
do
  	for PROC in ${PROCS_MATMUL};
        do
          	FILE=${RESULTDIR}/matmul_1_${N}_${PROC}

                if [ ! -f ${FILE} ]
                then
                qsub -d $(pwd) -q mamba -l mem=120gb,procs=${PROC} -v SCHED=1,N=${N},PROC=${PROC} ./run_matmul_weak.sh
                fi
        done
done


