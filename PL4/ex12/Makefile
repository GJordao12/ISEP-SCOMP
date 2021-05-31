cliente: cliente.o
	gcc -Wall -g cliente.o -o cliente.bin -lrt -lpthread

cliente.o: cliente.c
	gcc -Wall -g -c cliente.c

rcliente: cliente
	./cliente.bin

vendedor: vendedor.o
	gcc -Wall -g vendedor.o -o vendedor.bin -lrt -lpthread

vendedor.o: vendedor.c
	gcc -Wall -g -c vendedor.c

rvendedor: vendedor
	./vendedor.bin

clean:
	rm *.o *.bin