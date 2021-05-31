ex10: ex10.o
	gcc -Wall -g ex10.o -o ex10.bin -lrt -lpthread

ex10.o: ex10.c
	gcc -Wall -g -c ex10.c

run: ex10
	./ex10.bin

clean:
	rm *.o *.bin