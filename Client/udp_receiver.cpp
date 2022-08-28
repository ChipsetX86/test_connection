#include "udp_receiver.h"

#include <stdint.h>

#include <asio.hpp>

using namespace std;
using namespace std::chrono;
using asio::ip::udp;

struct udp_sender::pimpl {
    pimpl(const uint16_t port): 
        socket(context, udp::endpoint(udp::v4(), port)) {};
    asio::io_context context; 
    udp::socket socket;
};

udp_receiver::udp_receiver(const uint16_t &port)
{

}

udp_receiver::~udp_receiver()
{
    m_pimpl->stop();
}