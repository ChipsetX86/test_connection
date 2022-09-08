#ifndef GET_TIME_H
#define GET_TIME_H

#include <chrono>
#include <time.h>
#include <stdio.h>
#include <string>

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
	std::tm now = *gmtime(&tt);   
	char res[14] = { 0 };
	sprintf(res, "%02d:%02d:%02d.%03d", now.tm_hour, now.tm_min, now.tm_sec, static_cast<int>(milli % 100));
	return std::string(res);
}

#endif