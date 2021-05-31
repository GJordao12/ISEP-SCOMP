ex06: ex06.o
	gcc -Wall -g ex06.o -o ex06.bin -lrt

ex06.o: ex06.c
	gcc -Wall -g -c ex06.c

run: ex06
	./ex06.bin

clean:
	rm *.o *.bin