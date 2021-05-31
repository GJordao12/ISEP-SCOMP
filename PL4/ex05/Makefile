ex05: ex05.o
	gcc -Wall -g ex05.o -o ex05.bin -lrt -lpthread

ex05.o: ex05.c
	gcc -Wall -g -c ex05.c

run: ex05
	./ex05.bin

clean:
	rm *.o *.bin