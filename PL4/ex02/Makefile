ex02: ex02.o
	gcc -Wall -g ex02.o -o ex02.bin -lrt -lpthread

ex02.o: ex02.c
	gcc -Wall -g -c ex02.c

run: ex02
	./ex02.bin

clean:
	rm *.o *.bin