GCC = g++
FLAGS = -O3 -std=c++17

sh-fano: sh-fano.o
	${GCC} ${FLAGS} sh-fano.o -o sh-fano

sh-fano.o: sh-fano.cpp bitwriter.h
	${GCC} ${FLAGS} -c sh-fano.cpp -o sh-fano.o