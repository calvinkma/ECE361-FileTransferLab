CC=gcc
all: server deliver server_test round_trip_time_test
server: server.o
deliver: deliver.o
clean:
	rm -f *.o server deliver server_test server-* round_trip_time_test