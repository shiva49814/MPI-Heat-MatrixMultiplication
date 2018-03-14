#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

for N in ${N_matmul_strong};
do 
	FILE=${RESULTDIR}/heat_1_${N}
	    
	if [ ! -f ${FILE} ]
	then
	qsub -d $(pwd) -q mamba -l mem=50GB -l procs=1 -v N=${N} ./run_matmul.sh
	fi
done
