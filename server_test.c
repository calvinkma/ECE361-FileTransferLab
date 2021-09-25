#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MAXLINE 1024 // The maximum number of bytes that can be received from one UDP packet

/*
    This file tests the functionality of server.c by sending three packets from one file.
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

    server_port = (unsigned short)(atoi(argv[2]));

    // printf("Server address binary: %lu\n", server_addr_binary);
    // printf("Server port: %u\n", server_port);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = server_addr_binary;

    // Create test messages
    char frag0[23] = "3:0:6:foobar.txt:H Worl";
    char frag1[23] = "3:1:6:foobar.txt:d\0from";
    char frag2[21] = "3:2:6:foobar.txt:Test";

    // printf("socket_fd: %d\n", socket_fd);
    // printf("message: %s\n", message);
    // printf("strlen(message): %d\n", strlen(message));

    // Send a message
    int n_bytes_sent = sendto(socket_fd, (char *)frag0, 23, 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    printf("%d bytes sent succesfully.\n", n_bytes_sent);

    n_bytes_sent = sendto(socket_fd, (char *)frag1, 23, 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    printf("%d bytes sent succesfully.\n", n_bytes_sent);

    n_bytes_sent = sendto(socket_fd, (char *)frag2, 21, 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    printf("%d bytes sent succesfully.\n", n_bytes_sent);

    // Receive 3 messages
    for (int i = 0; i < 3; i++) {
        int n_bytes_received;
        char buffer[MAXLINE];
        int server_addr_size;
        n_bytes_received = recvfrom(socket_fd, buffer, MAXLINE, 0, (struct sockaddr *)(&server_addr), &server_addr_size);
        buffer[n_bytes_received] = '\0';

        printf("Server : %s\n", buffer);
    }

    return 0;
}
