//
// Created by adam slaymark on 20/02/2024.
//
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "socket.h"

/*Variables*/
static int client_socket;
static struct sockaddr_in server_addr;

#ifdef _WIN32
    WSADATA wsa;
#endif

int initialiseSocket(char* server_ip, int16_t PORT){
    // Set up socket
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket failed");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }
    return 1;
}

void sendDataSocket(){
    const char payload = 6; // Single byte payload
    // Send single datagram packet with 1 byte payload
    ssize_t send_len = sendto(client_socket, &payload, sizeof(payload), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (send_len < 0) {
        perror("sendto failed");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    printf("Sent a single datagram packet with a 1 byte payload\n");

#ifdef _WIN32
    Sleep(1000); // Sleep for 1 second
#else
    sleep(1); // Sleep for 1 second
#endif
}

void closeSocket(){
    close(client_socket);
#ifdef _WIN32
    WSACleanup();
#endif
}
