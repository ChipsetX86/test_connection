#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#pragma once

#include <stdint.h>
#include <memory>

#include "../common/message.h"

class udp_receiver
{
public:
    udp_receiver(const uint16_t &port);
    ~udp_receiver();

    void start();
    bool get_message(udp_message &);

private:
    udp_receiver(udp_receiver const & ) = delete;
    udp_receiver& operator=(udp_receiver const &) = delete;

    struct pimpl;
    std::unique_ptr<pimpl> m_pimpl;

    void receive();
};

#endif