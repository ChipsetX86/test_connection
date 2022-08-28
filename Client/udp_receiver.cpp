#include "udp_receiver.h"

#include <iostream>
#include <stdint.h>

#include <asio.hpp>

using namespace std;
using namespace std::chrono;
using asio::ip::udp;

const size_t SIZE_BUFFER = 100'000;

struct udp_receiver::pimpl {
    pimpl(const uint16_t port): 
        socket(context, udp::endpoint(udp::v4(), port)) {};
    asio::io_context context; 
    udp::socket socket;
    udp::endpoint endpoint;
    uint8_t buffer[SIZE_BUFFER];
};

udp_receiver::udp_receiver(const uint16_t &port):
    m_pimpl{make_unique<pimpl>(port)}
{
}

void udp_receiver::receive()
{
    m_pimpl->socket.async_receive_from(
        asio::buffer(m_pimpl->buffer, SIZE_BUFFER), 
        m_pimpl->endpoint, 
        [this](std::error_code ec, std::size_t bytes_recvd) {
                receive();

            });
} 

void udp_receiver::start()
{                                                  
  try {
    receive();
    m_pimpl->context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

udp_receiver::~udp_receiver()
{
    m_pimpl->context.stop();
}