ex13: ex13.o
	gcc -Wall -g ex13.o -o ex13.bin -lrt

ex13.o: ex13.c
	gcc -Wall -g -c ex13.c

run: ex13
	./ex13.bin

clean:
	rm *.o *.bin