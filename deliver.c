#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    short server_port;
    char file_name[30];
    FILE *file;
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
    if (socket_fd = socket(AF_INET, SOCK_DGRAM, 0) < 0) {
        printf("Error: Failed to create socket.\n");
        exit(EXIT_FAILURE);
    }

    // Get the server address and port number from the input arguments
    success = inet_pton(AF_INET, argv[1], &server_addr_binary);
    if (success != 1) {
        printf("Error: Invalid server address.\n");
        exit(EXIT_FAILURE);
    }
    server_port = (short)(atoi(argv[2]));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = htonl(server_addr_binary);

    // Prompt the user to enter a file name to search for
    printf("ftp ");
    scanf("%s", file_name);
    printf("Searching for file %s\n", file_name);

    if (access( file_name, F_OK ) == 0) {
        // File found
        printf("File %s found\n", file_name);
        strcpy(message, "ftp");
    } else {
        // File not found
        printf("File %s not found, exiting\n", file_name);
    }

    // Send a message 
    sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

    return 0;
}