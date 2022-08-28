#include "udp_sender.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <asio.hpp>

#include "md5/md5.h"

using namespace std;
using namespace std::chrono;
using asio::ip::udp;

const auto COUNT_BATCH = 2;
const auto BATCH_SIZE = 1000;
const auto INTERVAL_SEND_PACKET = 10ms;
const auto SLEEP_TIME = 10s;
const auto MIN_COUNT_BYTES = 600;
const auto MAX_COUNT_BYTES = 1600;

time_t get_time_stamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
		std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	time_t timestamp = tmp.count();
	return timestamp;
}

std::string format_time(time_t timestamp)
{
	uint64_t milli = timestamp + 8 * 60 * 60 * 1000;
	auto mTime = std::chrono::milliseconds(milli);
	auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
	auto tt = std::chrono::system_clock::to_time_t(tp);
	std::tm now;
	gmtime_s(&now, &tt);   
	char res[14] = { 0 };
	sprintf_s(res, sizeof(res), "%02d:%02d:%02d.%03d", now.tm_hour, now.tm_min, now.tm_sec, static_cast<int>(milli % 100));
	return std::string(res);
}

using raw_data = vector<uint8_t>;

struct message {
    uint64_t number;
    time_t time_ms;
    raw_data payload;
    string md5_payload;
    raw_data to_raw_data() const {
        raw_data result;
        auto size_payload = payload.size() * sizeof(decltype(*payload.data()));
        auto size_md5 =  md5_payload.size() * sizeof(decltype(*md5_payload.data()));
        result.resize(sizeof(number) + 
                      sizeof(time_ms) +
                      sizeof(size_t) + 
                      size_payload + 
                      size_md5);
        auto ptr = result.data();
        memcpy(ptr, &number, sizeof(number));
        ptr += sizeof(number);
        memcpy(ptr, &time_ms, sizeof(time_ms));
        ptr += sizeof(time_ms);
        memcpy(ptr, &size_payload, sizeof(size_payload));
        ptr += sizeof(size_payload);
        memcpy(ptr, payload.data(), size_payload);
        ptr += sizeof(size_payload);
        memcpy(ptr, md5_payload.data(), size_md5);
        ptr += sizeof(size_md5);
        return result;              
    }
};

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
            message m;
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