# SRC=test_mem_alloc.c mem_alloc.c mem_alloc.h Config.h
TARGET=test_mem_alloc
SRC=$(wildcard *.c)

$(TARGET):$(SRC)
	gcc -g $(SRC) -o $@

.PHONY:clean run

clean:
	-rm -f *.o *.exe

run:
	./$(TARGET) > a.txt
