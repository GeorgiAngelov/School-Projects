#!/bin/sh

#PBS -N word_counter
#PBS -l nodes=4,walltime=00:01:00
#PBS -M gaatyd@mail.missouri.edu
#PBS -m abe

module load openmpi-x86_64
mpirun /cluster/students/d/gaatyd/Homeworks/HPC/mpi/word_counter /cluster/students/d/gaatyd/Homeworks/HPC/mpi/genesis.txt
