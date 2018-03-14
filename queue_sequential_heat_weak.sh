
#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

for N in ${N_WEAK};
do
  	for PROC in ${PROCS};
        do
          	FILE=${RESULTDIR}/heat_weak_${N}_${PROC}

                if [ ! -f ${FILE} ]
                then
                qsub -d $(pwd) -q mamba -l mem=120gb,procs=${PROC} -v SCHED=1,N=${N},PROC=${PROC} ./run_heat_weak.sh
                fi
        done
done

