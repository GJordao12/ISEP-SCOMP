bridge: bridge.o
	gcc -Wall -g bridge.o -o bridge.bin -lrt -lpthread

bridge.o: bridge.c
	gcc -Wall -g -c bridge.c

run: bridge
	./bridge.bin

clean:
	rm *.o *.bin