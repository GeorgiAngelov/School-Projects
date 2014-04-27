#!/bin/sh

#PBS -N MPI_HW5_OUTPUT
#PBS -l nodes=6,walltime=00:40:00
#PBS -M gaatyd@mizzou.edu
#PBS -m abe

module load openmpi-x86_64
cd /cluster/students/d/gaatyd/Homeworks/HPC/gaatyd_hw5/
mpirun hw5 >> results.log Georgi
