#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include "udp_sender.h"

const auto HOST_FOR_SENDING = "127.0.0.1";
const uint16_t PORT_HOST_FOR_SENDING = 50'000;

int main(int argc, char *argv[])
{
    std::cout << "Start application send packets" << std::endl;

    udp_sender sender(HOST_FOR_SENDING, PORT_HOST_FOR_SENDING);
    sender.start();

    std::cout << "Close application send packets" << std::endl;
    return EXIT_SUCCESS;
}