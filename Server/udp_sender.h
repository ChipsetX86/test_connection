#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#pragma once

#include <memory>
#include <string>
#include <stdint.h>

class udp_sender
{
public:
    udp_sender(const std::string &address, uint16_t port);
    ~udp_sender();

    void start();

private:
     struct pimpl;
     std::unique_ptr<pimpl> m_pimpl;
};

#endif