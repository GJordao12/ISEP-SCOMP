ex04: ex04.o
	gcc -Wall -g ex04.o -o ex04.bin -lrt -lpthread

ex04.o: ex04.c
	gcc -Wall -g -c ex04.c

run: ex04
	./ex04.bin

clean:
	rm *.o *.bin