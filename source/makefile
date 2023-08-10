all : operateArrays

operateArrays : operateArrays.o
	g++ -o operateArrays operateArrays.o -lpthread -lm

operateArrays.o : operateArrays.cpp
	g++ -c -std=c++11 operateArrays.cpp -lm -lpthread

clean :
	rm *.o operateArrays