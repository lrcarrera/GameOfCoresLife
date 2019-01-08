#!/bin/bash

## Specifies the interpreting shell for the job.
#$ -S /bin/bash
#$ -q all.q
## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Execute the job from the current working directory.
#$ -cwd

## Parallel programming environment (mpich) to instantiate and number of computing slots.
#$ -pe mpich 12

## The  name  of  the  job.
#$ -N Practica

MPICH_MACHINES=$TMPDIR/mpich_machines
cat $PE_HOSTFILE | awk '{print $1":"$2}' > $MPICH_MACHINES


## In this line you have to write the command that will execute your application.
mpicc -o juego_vida JuegoVida_Radu.c
#mpiexec -f $MPICH_MACHINES -n $NSLOTS /home/rs6/Practica/juego_vida laba.txt  10 10 30 >> juego_vida.txt
#mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida.txt
#mpiexec -f $MPICH_MACHINES -n $NSLOTS /home/rs6/Practica/juego_vida >> juego_vida.txt

#10x10
#mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 2 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 3 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 5 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 4 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 6 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 7 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 8 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 9 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 10 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 11 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt
#mpiexec -f $MPICH_MACHINES -n 12 /home/rs6/Practica/juego_vida LifeGameInit_10x10_iter0.txt  10 10 150 >> juego_vida_10x10.txt

#80x140
#mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 2 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 3 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 4 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 6 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 5 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 7 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 8 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 10 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 9 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 11 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt
#mpiexec -f $MPICH_MACHINES -n 12 /home/rs6/Practica/juego_vida LifeGame_80x140_iter10.txt  140 80 150 >> juego_vida_80x140.txt

#200x400
#mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 2 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 3 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 4 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 6 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 5 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 7 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 8 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 9 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 10 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 11 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt
#mpiexec -f $MPICH_MACHINES -n 12 /home/rs6/Practica/juego_vida LifeGame_200x400_iter25.txt  400 200 150 >> juego_vida_200x400.txt

#1000x1000
#mpiexec -f $MPICH_MACHINES -n 1 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 1500 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 2 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 3 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 4 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 5 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 6 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 7 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 8 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 10 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 9 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
#mpiexec -f $MPICH_MACHINES -n 11 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt
mpiexec -f $MPICH_MACHINES -n 12 /home/rs6/Practica/juego_vida LifeGame_1000x1000_iter25.txt  1000 1000 750 >> juego_vida_1000x1000.txt




rm -rf $MPICH_MACHINES
