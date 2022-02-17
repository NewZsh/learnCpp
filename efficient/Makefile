# FLAGS
CC       = gcc
CXX      = g++
FINAL_LIBS = 
FINAL_FLAGS = -std=c++11 -Wall -W -Wno-missing-field-initializers -O2 -ggdb

# all: main
# main: math_op.o stack.o queue.o
# 	gcc math_op.o stack.o queue.o -o main

../test/search.benchmark: mysearch.cpp
	$(CXX) $(FINAL_FLAGS) -D __TEST_MYSEARCH -c mysearch.cpp -o mysearch.o
	$(CXX) $(FINAL_FLAGS) -o ../test/search.benchmark mysearch.o $(FINAL_LIBS)

clean:
	rm -rf main
	rm -rf *.o
	rm -rf *.benchmark
