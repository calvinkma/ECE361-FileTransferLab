CC=gcc
all: server deliver server_test
server: server.o
deliver: deliver.o
clean:
	rm -f *.o server deliver server_test