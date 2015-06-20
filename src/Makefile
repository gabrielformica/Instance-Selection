GPPFLAGS= -std=c++11
CLANG= clang++
CC= clang++
OBJ= main.cpp is.cpp metaheuristic.cpp tweaker.cpp 

clang: CC=clang++
clang: $(OBJ)
	$(CC) $(OBJ) $(GPPFLAGS) -o instance_selection 

gpp: CC=g++
gpp: $(OBJ)
	$(CC) $(OBJ) $(GPPFLAGS) -o instance_selection 

clean:
	rm -f instance_selection 
