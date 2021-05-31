ex07: ex07.o
	gcc -Wall -g ex07.o -o ex07.bin -lrt -lpthread

ex07.o: ex07.c
	gcc -Wall -g -c ex07.c

run: ex07
	./ex07.bin

clean:
	rm *.o *.bin