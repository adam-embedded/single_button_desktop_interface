cmake_minimum_required(VERSION 3.26)
project(hearing_test_button C)

set(CMAKE_C_STANDARD 11)

include_directories(inc)

add_executable(hearing_test_button src/main.c
        src/serial.c
        src/device.c
        src/socket.c
)

# Check if building on Windows
if(WIN32)
    # Link against the Winsock library
    target_link_libraries(hearing_test_button PRIVATE ws2_32)
endif()