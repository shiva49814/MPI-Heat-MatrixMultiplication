#!/bin/sh

RESULTDIR=result/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

mpirun ./mat_mul ${N} ${SCHED} 2> ${RESULTDIR}/matmul_1_${N}_{PROC}  >/dev/null






