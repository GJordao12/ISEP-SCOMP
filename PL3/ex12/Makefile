ex12: ex12.o
	gcc -Wall -g ex12.o -o ex12.bin -lrt

ex12.o: ex12.c
	gcc -Wall -g -c ex12.c

run: ex12
	./ex12.bin

clean:
	rm *.o *.bin