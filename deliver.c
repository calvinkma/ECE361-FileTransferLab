#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "packet.h"


#define MAXLINE 1024 // The maximum number of bytes that can be received from one UDP packet

int main(int argc, char *argv[]) {

     // Check input, must be supplied exactly 2 arguments
    if (argc != 3) {
        printf("Error: Invalid input is provided.\n");
        exit(EXIT_FAILURE);
    }

    // Try creating a socket
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error: Failed to create socket.\n");
        exit(EXIT_FAILURE);
    }

    // Get the server address and port number from the input arguments
    short success;
    uint32_t server_addr_binary;
    short server_port;
    struct sockaddr_in server_addr;
    success = inet_pton(AF_INET, argv[1], &server_addr_binary);
    if (success != 1) {
        printf("Error: Invalid server address.\n");
        exit(EXIT_FAILURE);
    }
    server_port = (unsigned short)(atoi(argv[2]));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = server_addr_binary;


    // Prompt the user to enter a file name to search for
    char file_name[100];
    printf("ftp ");
    scanf("%s", file_name);
    printf("Searching for file %s.\n", file_name);

    if (access(file_name, F_OK) == 0) {
        // File found
        printf("File %s found.\n", file_name);
        // strcpy(message, "ftp");
    } else {
        // File not found
        printf("File %s not found, exiting.\n", file_name);
        exit(0);
    }

    // Parse file
    FILE *fp;
    unsigned int file_size;
    unsigned int total_frag;
    char total_frag_str[100];
    char message[1500];

    // Get total file size
    fp = fopen(file_name, "r");
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("File size: %d\n", file_size);

    total_frag = (file_size / 1000) + 1;
    unsigned int frag_no = 1;
    char frag_no_str[100];
    unsigned int remaining_bytes = file_size;
    char remaining_bytes_str[100];
    unsigned int non_data_len;
    unsigned int data_size;
    for (frag_no = 1; frag_no <= total_frag; frag_no++) {
        printf("Total frags: %u\n", total_frag);
        sprintf(total_frag_str, "%u", total_frag);
        strcpy(message, total_frag_str);
        strcat(message, ":");
        // printf("Packet: %s\n", message);

        printf("Frag no: %u\n", frag_no);
        sprintf(frag_no_str, "%u", frag_no);
        strcat(message, frag_no_str);
        strcat(message, ":");
        // printf("Packet: %s\n", message);

        if (remaining_bytes > 1000) {
            strcat(message, "1000:");
            data_size = 1000;
            remaining_bytes = remaining_bytes - 1000;
        } else {
            sprintf(remaining_bytes_str, "%u", remaining_bytes);
            strcat(message, remaining_bytes_str);
            strcat(message, ":");
            data_size = remaining_bytes;
        }

        strcat(message, file_name);
        strcat(message, ":");

        non_data_len = strlen(message);
        // printf("Data start index: %d\n", non_data_len);
        fread(&message[non_data_len], 1, data_size, (FILE*)fp);
        printf("Packet: ");
	int i = 0;
	for (i = 0; i < non_data_len + data_size; i++) {
	    // printf("%d", i);
	    printf("%c", message[i]);
	}
	printf("\n");

        // Send a message
        int n_bytes_sent = sendto(socket_fd, (char *)message, non_data_len + data_size, 0, (struct sockaddr*)(&server_addr), sizeof(server_addr));
        printf("%d bytes sent succesfully.\n", n_bytes_sent);

        // Receive one message
        int n_bytes_received;
        char buffer[MAXLINE];
        int server_addr_size;
        n_bytes_received = recvfrom(socket_fd, buffer, MAXLINE, 0, (struct sockaddr *)(&server_addr), &server_addr_size);
        buffer[n_bytes_received] = '\0';

        // printf("Server : %s\n", buffer);

        /* === @JERRY: HACK INSERTED HERE: START === */
        buffer[3] = '\0';
        /* === @JERRY: HACK INSERTED HERE: END === */

        if (strcmp(buffer, "ACK") == 0) {
            printf("Recieved ACK from server.\n", buffer);
        } else {
            printf("Did not receive ACK from server, exiting.\n", buffer);
            exit(0);
        }
    }

    return 0;
}
