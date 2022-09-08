#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <cstring>

using raw_data = std::vector<uint8_t>;

struct udp_message {
    uint64_t number = 0;
    time_t time_ms = 0;
    raw_data payload;
    std::string md5_payload;
    void clear() {
        number = 0;
        time_ms = 0;
        md5_payload.clear();
        payload.clear();
    }
    bool empty() const {
        return !number && payload.empty();
    }
    raw_data to_raw_data() const {
        raw_data result;
        auto size_payload = payload.size() * sizeof(decltype(payload)::value_type);
        auto size_md5 =  md5_payload.size() * sizeof(decltype(md5_payload)::value_type);
        result.resize(sizeof(number) + 
                      sizeof(time_ms) +
                      sizeof(size_t) + 
                      size_payload + 
                      size_md5);
        auto ptr = result.data();
        std::memcpy(ptr, &number, sizeof(number));
        ptr += sizeof(number);
        memcpy(ptr, &time_ms, sizeof(time_ms));
        ptr += sizeof(time_ms);
        memcpy(ptr, &size_payload, sizeof(size_payload));
        ptr += sizeof(size_payload);
        memcpy(ptr, payload.data(), size_payload);
        ptr += size_payload;
        memcpy(ptr, md5_payload.data(), size_md5);
        ptr += size_md5;
        return result;              
    }

    bool from_raw_data(const raw_data &data) {
        if (data.size() < MIN_SIZE) {
            return false;
        }

        auto ptr = data.data();
        std::memcpy(&number, ptr, sizeof(number));
        ptr += sizeof(number);

        memcpy(&time_ms, ptr, sizeof(time_ms));
        ptr += sizeof(time_ms);

        size_t size_payload = 0;
        memcpy(&size_payload, ptr, sizeof(size_payload));
        ptr += sizeof(size_payload);

        if (data.size() <  MIN_SIZE + size_payload) {
            return false;
        }

        payload.resize(size_payload);
        memcpy(payload.data(), ptr, size_payload);
        ptr += size_payload;

        size_t size_md5 = data.data() + data.size() - ptr;
        if (size_md5 <= 0) {
            return false;
        } 
        md5_payload.resize(size_md5);
        std::memcpy(md5_payload.data(), ptr, size_md5);

        return true;
    }

    constexpr static auto MIN_SIZE = sizeof(number) + sizeof(time_ms) + sizeof(size_t) + 1;
};


#endif