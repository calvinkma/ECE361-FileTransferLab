#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>


#define MAXLINE 1024 // The maximum number of bytes that can be received from one UDP packet

/*
    This file tests the round trip time by ending one packet to server, and waiting for an `ACK` reply.
    Run the test by `./server_test <server_ip> <server_port_number>`, e.g. `./server_test 128.100.13.158 3000`.
    On the server side, a file `foobar.txt` should be created.
*/
int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    short server_port;
    char file_name[30];
    int socket_fd;
    char message[10];
    uint32_t server_addr_binary;
    short success;
    struct timeval tval_before, tval_after, tval_result;

     // Check input, must be supplied exactly 2 arguments
    if (argc != 3) {
        printf("Error: Invalid input is provided.\n");
        exit(EXIT_FAILURE);
    }

    // Try creating a socket
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error: Failed to create socket.\n");
        exit(EXIT_FAILURE);
    }

    // printf("socket_fd: %d\n", socket_fd);

    // Get the server address and port number from the input arguments
    success = inet_pton(AF_INET, argv[1], &server_addr_binary);
    if (success != 1) {
        printf("Error: Invalid server address.\n");
        exit(EXIT_FAILURE);
    }

    // Prepare metadata for sending
    server_port = (unsigned short)(atoi(argv[2]));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = server_addr_binary;
    char frag0[23] = "1:1:6:foobar.txt:H Worl";

    // Prepare metadata for receiving
    int n_bytes_received;
    char buffer[MAXLINE];
    int server_addr_size;

    // Start timer
    gettimeofday(&tval_before, NULL);

    // Send a message
    int n_bytes_sent = sendto(socket_fd, (char *)frag0, 23, 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    printf("%d bytes sent succesfully.\n", n_bytes_sent);


    // Receive a message
    n_bytes_received = recvfrom(socket_fd, buffer, MAXLINE, 0, (struct sockaddr *)(&server_addr), &server_addr_size);

    // End timer
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld seconds.\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    
    // Process received message
    buffer[n_bytes_received] = '\0';
    printf("Server : %s\n", buffer);

    return 0;
}
