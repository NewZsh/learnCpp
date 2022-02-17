# FLAGS
CC       = gcc
CXX      = g++
FINAL_FLAGS = -pedantic -DREDIS_STATIC='' -std=c++11 -Wall -W -Wno-missing-field-initializers -O2 -g -ggdb -DUSE_JEMALLOC -I../languageToolkit/deps/jemalloc/include
FINAL_LIBS = ../languageToolkit/deps/jemalloc/lib/libjemalloc.a -lm -ldl -pthread -lrt

# all: main
# main: math_op.o stack.o queue.o
# 	gcc math_op.o stack.o queue.o -o main

mathexp-benchmark: math_op.cpp stack.cpp queue.cpp zmalloc.c
	$(CC) $(FINAL_FLAGS) -c zmalloc.c -o zmalloc.o
	$(CXX) $(FINAL_FLAGS) -c stack.cpp -o stack.o
	$(CXX) $(FINAL_FLAGS) -c queue.cpp -o queue.o
	$(CXX) $(FINAL_FLAGS) -D __TEST_MATHEXP -c math_op.cpp -o math_op.o
	$(CXX) $(FINAL_FLAGS) -o mathexp-benchmark math_op.o queue.o zmalloc.o stack.o $(FINAL_LIBS)
stack-benchmark: stack.cpp zmalloc.c
	$(CC) $(FINAL_FLAGS) -c zmalloc.c -o zmalloc.o
	$(CXX) $(FINAL_FLAGS) -D __TEST_STACK -c stack.cpp -o stack.o
	$(CXX) $(FINAL_FLAGS) -o stack-benchmark zmalloc.o stack.o $(FINAL_LIBS)
list-benchmark: list.cpp zmalloc.c
	$(CC) $(FINAL_FLAGS) -c zmalloc.c -o zmalloc.o
	$(CXX) $(FINAL_FLAGS) -D __TEST_LIST -c list.cpp -o list.o
	$(CXX) $(FINAL_FLAGS) -o list-benchmark zmalloc.o list.o $(FINAL_LIBS) 
queue-benchmark: queue.cpp zmalloc.c
	$(CC) $(FINAL_FLAGS) -c zmalloc.c -o zmalloc.o
	$(CXX) $(FINAL_FLAGS) -D __TEST_QUEUE -c queue.cpp -o queue.o
	$(CXX) $(FINAL_FLAGS) -o queue-benchmark zmalloc.o queue.o $(FINAL_LIBS)

clean:
	rm -rf *.o main
