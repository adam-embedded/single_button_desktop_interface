#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>

/* Private Includes*/
#include "main.h"
#include "serial.h"
#include "device.h"
#include "socket.h"
#include "ANSI_colors.h"

#ifdef _WIN32
#include <windows.h>

/* Volatile Variables */
volatile BOOL stop = FALSE;

BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType) {
    switch (ctrlType) {
        case CTRL_C_EVENT:
            printf("Ctrl+C (SIGINT) received.\n");
            stop = TRUE;
            return TRUE;

        default:
            return FALSE;
        }
}
#elif __unix__ || __APPLE__
/* Volatile Variables */
volatile sig_atomic_t stop = 0;

static void handle_sig(int sig)
{
    printf("Waiting for process to finish... got signal : %d", sig);
    stop = 1;
}
#endif

int main(int argc, char** argv) {
    int16_t PORT = 0;
    char serialName[256];
    int32_t baud = 0;

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
#ifdef _WIN32
// Set console control handler
//    if (!SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE)) {
//        fprintf(stderr, "Error setting console control handler.\n");
//        return 1;
//    }

#elif __unix__ || __APPLE__
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
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("Failed to register signal handler");
        return 1;
    }

#endif

    // Begin Program
    printf(ANSI_COLOR_ORANGE "%s\n" ANSI_COLOR_RESET, welcomeLogo);


    printf(ANSI_COLOR_BLUE "Welcome to the button controller gateway\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "Please wait for device detection...\n" ANSI_COLOR_RESET);

///////
/* Serial Setup */
///////
    //Initialise Serial - If there is an issue the library will self terminate the program
    serialInit(serialName, baud);
///////
/* End Serial Setup */
/////

///////
/*Device Setup*/
///////
    //Check for device online and initialise device
    initialiseDevice();
    // set up function endpoints for button press
    allocateButtonFunc(sendDataSocket);

//////
/*End Device Setup*/
//////

//////
/* UDP Setup */
//////
    char server_ip[] = "127.0.0.1";
    initialiseSocket(server_ip, PORT);
///////
/* End socket setup */
///////

    // Start button sampling
    buttonStart();
    usleep(150000L);
    printf(ANSI_COLOR_MAGENTA"The terminal will display the first 10 button changes:\n\n"ANSI_COLOR_RESET);


    // Loop
    while (!stop) {

        DeviceLoop();
        usleep(50000L);
    }

    printf("* Stopping\n");

    // Close socket
    closeSocket();
    closeSerial();


    printf("* Exiting\n");

    return 0;
}
