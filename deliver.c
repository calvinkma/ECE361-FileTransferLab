#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MAXLINE 1024 // The maximum number of bytes that can be received from one UDP packet

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

    // Prompt the user to enter a file name to search for
    printf("ftp ");
    scanf("%s", file_name);
    printf("Searching for file %s.\n", file_name);

    if (access( file_name, F_OK ) == 0) {
        // File found
        printf("File %s found.\n", file_name);
        strcpy(message, "ftp");
    } else {
        // File not found
        printf("File %s not found, exiting.\n", file_name);
        exit(0);
    }

    // printf("socket_fd: %d\n", socket_fd);
    // printf("message: %s\n", message);
    // printf("strlen(message): %d\n", strlen(message));

    // Send a message
    int n_bytes_sent = sendto(socket_fd, (char *)message, strlen(message), 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    printf("%d bytes sent succesfully.\n", n_bytes_sent);

    // Receive one message
    int n_bytes_received;
    char buffer[MAXLINE];
    int server_addr_size;
    n_bytes_received = recvfrom(socket_fd, buffer, MAXLINE, 0, (struct sockaddr *)(&server_addr), &server_addr_size);
    buffer[n_bytes_received] = '\0';

    // printf("Server : %s\n", buffer);

    if (strcmp(buffer, "yes") == 0) {
        printf("A file transfer can start.\n", buffer);
    } else {
        printf("Did not receive yes from server, exiting.\n", buffer);
        exit(0);
    }

    return 0;
}
