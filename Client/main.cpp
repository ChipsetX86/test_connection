#include <iostream>
#include <stdlib.h>

#include "udp_receiver.h"

const uint16_t PORT_HOST_FOR_RECEIVER = 50'000;

int main(int argc, char *argv[])
{
    std::cout << "Start application receiver packets" << std::endl;

    udp_receiver receiver(PORT_HOST_FOR_RECEIVER);
    receiver.start();

    std::cout << "Close application receiver packets" << std::endl;
    return EXIT_SUCCESS;
}