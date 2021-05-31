exame: exame.o
	gcc -Wall -g exame.o -o exame.bin -lrt -lpthread

exame.o: exame.c
	gcc -Wall -g -c exame.c

run: exame
	./exame.bin

clean:
	rm *.o *.bin