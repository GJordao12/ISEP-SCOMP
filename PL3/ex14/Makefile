ex14: ex14.o
	gcc -Wall -g ex14.o -o ex14.bin -lrt

ex14.o: ex14.c
	gcc -Wall -g -c ex14.c

run: ex14
	./ex14.bin

clean:
	rm *.o *.bin