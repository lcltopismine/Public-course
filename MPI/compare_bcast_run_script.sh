#! /bin/bash

# ====== Windows =======#

PBS -l nodes=1:ppn=2
# mpiexec -n ${PROCESS_COUNT} ./compare_bcast $2 $3
mpiexec -n 2 ./compare_bcast 5 10

# declare a name for this job to be sample_job
#PBS -N qcheng1
# request the parallel queue for this job
#PBS -q mamba
# request a total of 6 processors for this job (3 nodes and 2 processors per node)
#PBS -l nodes=3:ppn=2
# request 4 hours of cpu time
#PBS -l cput=0:04:00
# combine PBS standard output and error files
#PBS -j oe
# mail is sent to you when the job starts and when it terminates or aborts
#PBS -m bea
# specify your email address
#PBS -M cli33@uncc.edu

exec -n 2 ./compare_bcast 5 10

exit 0
