#include "processing.h"

#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>
#include <stdio.h>
#include <stdlib.h> 

#include "udp_receiver.h"
#include "../common/get_time.h"
#include "../third_party/md5/md5.h"

using namespace std;

const auto INTERVAL_PROCESSING = 15ms;


processing::processing(udp_receiver *receiver):
    m_receiver(receiver)
{

}

processing::~processing()
{

}

void processing::start()
{
    if (!m_receiver) {
        return;
    }

    auto receiver = m_receiver;
    thread t([&receiver]() {
        udp_message packet;
        while (1) {
            if (!receiver) {
                break;
            }
            packet.clear();
            if (receiver->get_message(packet)) {
                bool test = false;
                if (!packet.payload.empty()) {
                    test = md5(packet.payload.data(), packet.payload.size() * sizeof(decltype(packet.payload)::value_type)) == 
                                    packet.md5_payload;
                }
                std::cout << "Processed: #" << to_string(packet.number)  << " #" << format_time(get_time_stamp()) << (test ? " PASS" : " FAIL") << std::endl;                
            }
            this_thread::sleep_for(INTERVAL_PROCESSING);
        }
    });

    t.join();
}