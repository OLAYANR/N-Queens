N= 
nqueen :nq.o 
	gcc -o nqueen nq.o
		
nq.o : nq.c
	gcc -c nq.c

readme: 
	@echo "This program calculates the total number of solutions to the N Queens problem"\
	@echo "Terminal Usage: make run N= user pass in size of board -->  for example make run N=10 "\
	
run : 	
	./nqueen ${N}
	@echo "in case of error, please type : make readme"
	
.PHONY : clean

clean :
	-rm nqueen nq.o
