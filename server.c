#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
    server_addr -> sin_port = htons(0);
    return server_addr;
}

int main(int argc, char *argv[]) {
 printf("Hello World!\n");

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
 printf("UDP Port Opened on Port %hi. Listening...\n", udp_listen_port);

 // Listen
 int i = 0;
 while (1) {
     i += 1;
 }

 return 0;
}