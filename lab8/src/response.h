#ifndef RESPONSE_H
#define RESPONSE

#include <cstdint>

const unsigned int PAYLOAD=800;

typedef struct __attribute__((packed))response{
    uint16_t seq;
    uint32_t checksum;
    uint8_t payload[PAYLOAD];
	uint8_t flag;
    response();
    response(uint16_t,uint8_t*,unsigned);
    uint32_t genHash();
    bool check();
}response;

#endif
