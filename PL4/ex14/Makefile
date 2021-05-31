reader: reader.o
	gcc -Wall -g reader.o -o reader.bin -lrt -lpthread

reader.o: reader.c
	gcc -Wall -g -c reader.c

rreader: reader
	./reader.bin

writer: writer.o
	gcc -Wall -g writer.o -o writer.bin -lrt -lpthread

writer.o: writer.c
	gcc -Wall -g -c writer.c

rwriter: writer
	./writer.bin

clean:
	rm *.o *.bin