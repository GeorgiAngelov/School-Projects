#!/bin/sh

#PBS -N pi_estimator
#PBS -l nodes=25,walltime=00:01:00
#PBS -M kmm4mc@mizzou.edu
#PBS -m abe

module load openmpi-x86_64
mpirun /cluster/students/c/kmm4mc/mpi/pi_estimator
