#!/bin/sh

#PBS -N MPI_HW5_OUTPUT
#PBS -l nodes=4,walltime=00:01:00
#PBS -M gaatyd@mizzou.edu
#PBS -m abe

module load openmpi-x86_64
cd /cluster/students/d/gaatyd/Homeworks/HPC/gaatyd_hw5/
mpirun main Georgi
