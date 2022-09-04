#include "processing.h"

#include <iostream>
#include <thread>

#include "udp_receiver.h"
#include "../third_party/md5/md5.h"

using namespace std;

const auto INTERVAL_PROCESSING = 15ms;

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