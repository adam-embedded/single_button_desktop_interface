//
// Created by adam slaymark on 20/02/2024.
//
// Library for using serial with unix systems and windows systems

/* Includes */
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <fcntl.h>
    #include <termios.h>
    #include <unistd.h>
    #include <errno.h>
#endif

/* Private Includes */
#include "serial.h"



/* Static Variables */
#ifdef _WIN32
    static HANDLE SerialPort;
#elif  __unix__ || __APPLE__
    static int SerialPort;
#endif


/* Main Functions */

void serialInit(char* serPort, int baud){
#ifdef _WIN32
    SerialPort = CreateFile(serPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (SerialPort == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port\n");
        exit(1);
    }
    // Configure the serial port settings
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serial_port, &dcbSerialParams)) {
        printf("Error getting serial port state\n");
        CloseHandle(serial_port);
        exit(1);
    }
    dcbSerialParams.BaudRate = baud;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(serial_port, &dcbSerialParams)) {
        printf("Error setting serial port state\n");
        CloseHandle(serial_port);
        exit(1);
    }

#elif __unix__ || __APPLE__
    SerialPort = open(serPort, O_RDWR);
    if (SerialPort < 0) {
        perror("Error opening serial port");
        exit(EXIT_FAILURE);
    }

    // Configure the serial port settings (use the termios structure and ioctl function)
    struct termios options;
    if (tcgetattr(SerialPort, &options) != 0) {
        perror("Error getting serial port settings");
        exit(EXIT_FAILURE);
    }

    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    options.c_cflag &= ~PARENB; // Disable parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE; // Clear bit mask for data bits
    options.c_cflag |= CS8; // Set 8 data bits
    options.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore control lines
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    options.c_lflag = 0; // No signaling characters, no echo, no canonical processing
    options.c_oflag = 0; // No output processing

    if (tcsetattr(SerialPort, TCSANOW, &options) != 0) {
        perror("Error setting serial port attributes");
        exit(EXIT_FAILURE);
    }
#endif

    printf("Serial Device Initiated\n");
}

#ifdef _WIN32
    // Function to send a byte over UART
    void sendByteOverUART(HANDLE serial_port, unsigned char byte) {
        DWORD bytes_written;
        // Write the byte to the serial port
        if (!WriteFile(serial_port, &byte, 1, &bytes_written, NULL)) {
            printf("Error writing to serial port\n");
        }
    }

    // Function to receive a byte over UART
    unsigned char receiveByteOverUART(HANDLE serial_port) {
        DWORD bytes_read;
        unsigned char byte;
        // Read one byte from the serial port
        if (!ReadFile(serial_port, &byte, 1, &bytes_read, NULL)) {
            printf("Error reading from serial port\n");
            return 0; // Return 0 if there's an error
        }
        return byte;
    }
#elif defined(__unix__) || defined(__APPLE__)
// Function to send a byte over UART
void sendByteOverUART(int serial_port, unsigned char byte) {
    // Write the byte to the serial port
    if (write(serial_port, &byte, 1) != 1) {
        perror("Error writing to serial port");
    }
}

// Function to receive a byte over UART
unsigned char receiveByteOverUART(int serial_port) {
    unsigned char byte;
    // Read one byte from the serial port
    if (read(serial_port, &byte, 1) != 1) {
        perror("Error reading from serial port");
        return 0; // Return 0 if there's an error
    }
    return byte;
}
#endif

void closeSerial(){
#ifdef _WIN32
    CloseHandle(serial_port);
#elif defined(__unix__) || defined(__APPLE__)
    close(SerialPort);
#endif
}

uint8_t sendReceive(uint8_t message){
    sendByteOverUART(SerialPort, message);
    return receiveByteOverUART(SerialPort);

}

