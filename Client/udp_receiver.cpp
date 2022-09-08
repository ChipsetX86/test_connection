#include "udp_receiver.h"

#include <iostream>
#include <stdint.h>
#include <mutex>
#include <asio.hpp>

#include "../common/message.h"

using namespace std;
using namespace std::chrono;
using asio::ip::udp;

const size_t SIZE_BUFFER = 100'000;
const auto SIZE_RING_BUFFER = 16;

using raw_data = vector<uint8_t>;

struct udp_receiver::pimpl {
    pimpl(const uint16_t port): 
        socket(context, udp::endpoint(udp::v4(), port)),
        endpoint(udp::v4(), 0),
        ring_buffer(SIZE_RING_BUFFER)
        {};
    asio::io_context context; 
    udp::socket socket;
    udp::endpoint endpoint;
    uint8_t buffer[SIZE_BUFFER];

    vector<udp_message> ring_buffer;
    size_t index_write_ring_buffer = 0;
    size_t index_read_last_ring_buffer = 0;
    std::mutex mutex_ring_buffer;
    void add_message_in_ring_buffer(udp_message);
    bool get_message(udp_message &);
};

void udp_receiver::pimpl::add_message_in_ring_buffer(udp_message message)
{
    std::lock_guard<std::mutex> lck(mutex_ring_buffer);
    std::cout << "ADD PACKET " << std::to_string(message.number) << endl;
    ring_buffer[index_write_ring_buffer] = message;
    index_write_ring_buffer++;
    if (index_write_ring_buffer >= SIZE_RING_BUFFER) {
        index_write_ring_buffer = 0;
    }
}

udp_receiver::udp_receiver(const uint16_t &port):
    m_pimpl{make_unique<pimpl>(port)}
{
}

void udp_receiver::receive()
{
    m_pimpl->socket.async_receive_from(
        asio::buffer(m_pimpl->buffer, SIZE_BUFFER), 
        m_pimpl->endpoint, 
        [this](std::error_code ec, std::size_t bytes) {
                if (!bytes) {
                  receive();
                  return;
                }
                raw_data data;
                data.resize(bytes);
                memcpy(data.data(), m_pimpl->buffer, bytes);
                receive();
                udp_message message;
                message.from_raw_data(data);
                m_pimpl->add_message_in_ring_buffer(message);
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

bool udp_receiver::get_message(udp_message &mes)
{
  return m_pimpl->get_message(mes);
}

bool udp_receiver::pimpl::get_message(udp_message &mes)
{
  std::lock_guard<std::mutex> lck(mutex_ring_buffer);
  size_t counter = 0;
  while (ring_buffer[index_read_last_ring_buffer].empty() && counter < SIZE_RING_BUFFER) {
    index_read_last_ring_buffer++;
    if (index_read_last_ring_buffer >= SIZE_RING_BUFFER) {
      index_read_last_ring_buffer = 0;
    } 
    counter++;
  }

  if (ring_buffer[index_read_last_ring_buffer].empty()) {
      index_read_last_ring_buffer = index_write_ring_buffer;
      return false;
  }
  mes = ring_buffer[index_read_last_ring_buffer];
  ring_buffer[index_read_last_ring_buffer].clear();
  index_read_last_ring_buffer++;
  if (index_read_last_ring_buffer >= SIZE_RING_BUFFER) {
      index_read_last_ring_buffer = 0;
  }
  return true;
}