#ifndef REQUEST_H
#define REQUEST

#include <cstdint>
#include <bitset>
#include <vector>

using namespace std;

const int BS=3072;

typedef struct __attribute__((packed))request{
    uint32_t seq;
    uint32_t par;
	uint8_t flag;
    bitset<BS> bs;
    request();
    request(uint32_t);
    request(uint32_t,uint32_t);
    request(uint32_t,const vector<bool>&);
}request;

#endif
