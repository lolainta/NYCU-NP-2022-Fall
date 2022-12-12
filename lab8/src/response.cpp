#include "response.h"
#include <cstring>

response::response():flag(0){};

response::response(uint32_t _seq,uint8_t*_payload,unsigned offset):seq(_seq),flag(0){
    memcpy(payload,_payload+(long long)offset*PAYLOAD,PAYLOAD);
}


