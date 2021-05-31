ex01: ex01.o
	gcc -Wall -g ex01.o -o ex01.bin -lrt -lpthread

ex01.o: ex01.c
	gcc -Wall -g -c ex01.c

run: ex01
	./ex01.bin

clean:
	rm *.o *.bin