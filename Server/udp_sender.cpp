#include "udp_sender.h"

#include <iostream>
#include <memory>
#include <thread>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <cstring>

#include <asio.hpp>

#include "../third_party/md5/md5.h"
#include "../common/get_time.h"
#include "../common/message.h"

using namespace std;
using namespace std::chrono;
using asio::ip::udp;

constexpr auto COUNT_BATCH = 2;
constexpr auto BATCH_SIZE = 1000;
constexpr auto INTERVAL_SEND_PACKET = 10ms;
constexpr auto SLEEP_TIME = 10s;
constexpr auto MIN_COUNT_BYTES = 600;
constexpr auto MAX_COUNT_BYTES = 1600;

struct udp_sender::pimpl {
    pimpl(const std::string &address, uint16_t port): 
        resolver(context),
        socket(context, udp::endpoint(udp::v4(), 0)) {
            endpoint = *resolver.resolve(udp::v4(), address, to_string(port)).begin();
        };
    asio::io_context context; 
    udp::resolver resolver;
    udp::socket socket;
    udp::endpoint endpoint;
};

udp_sender::udp_sender(const std::string &address, uint16_t port):
    m_pimpl{make_unique<pimpl>(address, port)}
{
    
}

udp_sender::~udp_sender()
{
    m_pimpl->context.stop();
}

void udp_sender::start()
{
    asio::error_code ec;
    uint64_t counter = 0;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (auto k = 0; k < COUNT_BATCH; ++k) {
        for (auto i = 0; i < BATCH_SIZE; ++i) {
            udp_message m;
            m.number = counter++;
            m.payload.resize(rand() % (MAX_COUNT_BYTES + 1 - MIN_COUNT_BYTES) + MIN_COUNT_BYTES);
            for (size_t j = 0; j < m.payload.size(); j++) {
                m.payload[j] = rand();
            }
            m.time_ms = get_time_stamp();
            size_t payload_size =  m.payload.size() * sizeof(decltype(*m.payload.data()));
            m.md5_payload = md5(m.payload.data(), payload_size);

            auto packet_data = m.to_raw_data();
            m_pimpl->socket.send_to(asio::buffer(packet_data.data(), packet_data.size() * sizeof(decltype(*m.payload.data()))), 
                                    m_pimpl->endpoint, {}, ec);
            if (ec.value()) {
                std::cout << "Error sent packet" << std::endl;
            } else {
                std::cout << "Sent: #" << to_string(m.number)  << " #" << format_time(m.time_ms) << std::endl;
            }
            this_thread::sleep_for(INTERVAL_SEND_PACKET);
        }

        this_thread::sleep_for(SLEEP_TIME);
    }
}