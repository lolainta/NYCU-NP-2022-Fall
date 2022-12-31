#ifndef RESPONSE_H
#define RESPONSE

#include <cstdint>

typedef struct response{
    uint32_t seq;
    uint8_t payload[PAYLOAD];
	uint8_t flag;
    response();
    response(uint32_t,uint8_t*,unsigned);
}response;

#endif
