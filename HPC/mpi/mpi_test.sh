#!/bin/sh

#PBS -N mpi_test
#PBS -l nodes=4,walltime=00:01:00
#PBS -M gaatyd@mail.missouri.edu
#PBS -m abe

module load openmpi-x86_64
cd /cluster/students/d/gaatyd/Homeworks/HPC/mpi/
mpirun mpi_test Georgi