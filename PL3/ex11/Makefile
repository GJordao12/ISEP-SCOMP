ex11: ex11.o
	gcc -Wall -g ex11.o -o ex11.bin -lrt

ex11.o: ex11.c
	gcc -Wall -g -c ex11.c

run: ex11
	./ex11.bin

clean:
	rm *.o *.bin