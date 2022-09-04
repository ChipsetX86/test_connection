#include <iostream>
#include <stdlib.h>
#include <thread>
#include <atomic>

#include "udp_receiver.h"
#include "processing.h"

const uint16_t PORT_HOST_FOR_RECEIVER = 50'000;

static std::atomic<udp_receiver*> ptr_receiver = nullptr;

int main(int argc, char *argv[])
{
    std::cout << "Start application receiver packets" << std::endl;

    std::thread thread([]() {
        ptr_receiver = new udp_receiver(PORT_HOST_FOR_RECEIVER);
        ptr_receiver.load()->start();
    });

    while (!ptr_receiver.load()) {}
    
    processing proc(ptr_receiver);
    proc.start();

    thread.join();

    delete ptr_receiver;

    std::cout << "Close application receiver packets" << std::endl;
    return EXIT_SUCCESS;
}