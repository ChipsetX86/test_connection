#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#pragma once

#include <stdint.h>
#include <memory>

class udp_receiver
{
public:
    udp_receiver(const uint16_t &port);
    ~udp_receiver();

    void start();

private:
    struct pimpl;
    std::unique_ptr<pimpl> m_pimpl;
};

#endif