#!/bin/bash

## Specifies the interpreting shell for the job.
#$ -S /bin/bash
#$ -q all.q
## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Execute the job from the current working directory.
#$ -cwd

## Parallel programming environment (mpich) to instantiate and number of computing slots.
#$ -pe mpich 4

## The  name  of  the  job.
#$ -N Practica

MPICH_MACHINES=$TMPDIR/mpich_machines
cat $PE_HOSTFILE | awk '{print $1":"$2}' > $MPICH_MACHINES


## In this line you have to write the command that will execute your application.
mpicc -o juego_vida JuegoVida.c
mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida.txt
#mpiexec -f $MPICH_MACHINES -n $NSLOTS /home/rs6/Practica/juego_vida >> juego_vida.txt





rm -rf $MPICH_MACHINES
