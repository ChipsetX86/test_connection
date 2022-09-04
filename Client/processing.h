#ifndef PROCESSING_H
#define PROCESSING_H

#include <thread>

class udp_receiver;

class processing
{
public:
    processing(udp_receiver *receiver);
    ~processing();

    void start();
private:
    udp_receiver *m_receiver;
};

#endif