#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

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

/* Private Includes*/
#include "main.h"
#include "serial.h"
#include "device.h"

/* Variables */
volatile sig_atomic_t stop = 0;
static char serialName[256];
static int32_t baud = 0;

/* Prototypes */
static void handle_sig(int sig)
{
    printf("Waiting for process to finish... got signal : %d", sig);
    stop = 1;
}


int main(int argc, char** argv) {
    int16_t PORT = 0;

    // Parse options
    int index;
    int c;

    opterr = 0;

    //Get arguments
    while ((c = getopt (argc, argv, "s:p:b:")) != -1) {
        switch (c)
        {
            case 's': // serial
            {
                strncpy(serialName, optarg, sizeof(serialName));
                break;
            }
            case 'p':
            {
                PORT = atoi(optarg);
                break;
            }
            case 'b':
            {
                baud = atoi(optarg);
                break;
            }
            case 'q':
                atoi(optarg) == 2 ? setQos(MODE_2): setQos(MODE_1);
                break;
            case '?':

                if (optopt == 's')
                    fprintf (stderr, "Option -%s requires an argument.\n", optopt);
                else if (optopt == 'p')
                    fprintf (stderr, "Option -%s requires an argument.\n", optopt);
                else if (optopt == 'b')
                    fprintf (stderr, "Option -%s requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%s'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);

                fprintf(stderr, "Usage: -l 0..5 -s /tmp/enview.sock");
                return 1;
            default:
                break;
        }
    }

    //Handle signal
    struct sigaction sa;
    // Listen to ctrl+c and assert
    // Setup the signal handler
    sa.sa_handler = handle_sig;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Failed to register signal handler");
        return 1;
    }


    // Begin Program
    printf("Button Interface Started\n");
    printf("Please wait for device detection");

    ///////
    /* Serial Setup */
    ///////

    //Initialise Serial - If there is an issue the library will self terminate the program
    serialInit(serialName, baud);

    //Check for device online
    ReadyStatus();


    //////
    /* UDP Setup */
    /////

    // Set up socket
    int client_socket;
    struct sockaddr_in server_addr;
    const char *server_ip = "127.0.0.1";
    //const int PORT = 8080;
    const char payload = 6; // Single byte payload

#ifdef _WIN32
    WSADATA wsa;
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




    // Loop to send packets
    while (!stop) {
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

    printf("* Stopping\n");

    // Close socket
    close(client_socket);
    closeSerial();

#ifdef _WIN32
    WSACleanup();
#endif
    printf("* Exiting\n");

    return 0;
}
