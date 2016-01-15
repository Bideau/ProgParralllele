
MPICC   = mpicc 
CC 	= gcc
CFLAGS 	=  -I.. ./src/json.c ./src/myK.c ./src/graph4.c ./src/gnuplot_i.c -lm
RM	= rm -fr

default: mpi
		
mpi: 
	$(MPICC) $(CFLAGS) -o ./bin/mpiTest ./src/mpiMain.c

exec:
	mpirun -np 2 ./bin/mpiTest data.json 
clean:
	$(RM) ./result/*
	$(RM) ./bin/*

install:
	mkdir ./result
	mkdir ./bin

