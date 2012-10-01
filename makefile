OBJS = nq.o
CC = gcc
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
N= 

nqueen : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o nqueen
	
nq.o : nq.c
	$(CC) $(CFLAGS) nq.c

readme: 
	@echo "This program calculates the total number of solutions to the N Queens problem"\
	@echo "Terminal Usage: make run N= user pass in size of board -->  for example make run N=10 "\
	
run : 	
	./nqueen ${N}
	@echo "in case of error, please type : make readme"
	
.PHONY : clean

clean :
	-rm *.o *~ nqueen


