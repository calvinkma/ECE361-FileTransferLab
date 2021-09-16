#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
    Steps to send a UDP package from src to dest using Linux CLI:
    1. On dest side, run `hostname -I` to confirm IP address, e.g. "128.100.13.158"
    2. On dest side, run `./sever <port number>`, e.g. `./server 10034` to start server
    3. On src side, run e.g. `nc -u 128.100.13.158 10034` to establish UDP connection to dest
    4. On src side, send a message (e.g. enter "Hello world" on the same command prompt)
        * Note: This would be received as "Hello world\n" on the dest side.
    5. Verify behaviour on the dest side
*/


#define MAXLINE 1024 // The maximum number of bytes that can be received from one UDP packet

bool is_input_valid(int argc, char *argv[]) {
    return argc == 2; // Must be supplied exactly 1 argument
}

short get_udp_listen_port_from_input(char *argv[]) {
    return (short)(atoi(argv[1]));
}

int open_udp_socket(void) {
    return socket(AF_INET, SOCK_DGRAM, 0);
}

int bind_socket_to_port(int socket_fd, struct sockaddr_in* server_addr) {
    int status = bind(socket_fd, (struct sockaddr *) server_addr, sizeof(struct sockaddr));
    if (status != 0) {
        printf("Error: Failed to bind socket to port. Retry with another port.\n");
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in* get_server_addr(short port) {
    struct sockaddr_in* server_addr = (struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
    server_addr -> sin_family = AF_INET;
    server_addr -> sin_port = htons(port);
    (server_addr -> sin_addr).s_addr = htonl(INADDR_ANY);
    return server_addr;
}

bool is_message_ftp(char* message) {
    return strcmp(message, "ftp\n") == 0; // TODO: Remove the \n when not testing with the nc commands
}

int main(int argc, char *argv[]) {
    // Check input
    if (!is_input_valid(argc, argv)) {
        printf("Error: Invalid input is provided.\n");
        exit(EXIT_FAILURE);
    }

    // Get input arg
    short udp_listen_port = get_udp_listen_port_from_input(argv);

    // Initialize socket
    int socket = open_udp_socket();
    struct sockaddr_in* server_addr = get_server_addr(udp_listen_port);
    bind_socket_to_port(socket, server_addr);
    printf("UDP Port Opened on Port %hi. Listening...\n", udp_listen_port); // Run `ss -tulpn | grep ':3000'` to verify if there is a udp socket on Port 3000

    // Receive one message
    int n_bytes_received;
    char buffer[MAXLINE];
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    n_bytes_received = recvfrom(socket, buffer, MAXLINE, 0, (struct sockaddr *)(&client_addr), &client_addr_size);
    buffer[n_bytes_received] = '\0';
    printf("Client : %s\n", buffer);

    // Craft reply message
    char* message;
    if (is_message_ftp(buffer)) {
        message = "yes";
    } else {
        message = "no";
    }

    // Send reply
    sendto(socket, message, strlen(message), 0, (struct sockaddr *)(&client_addr), client_addr_size);

    return 0;
}