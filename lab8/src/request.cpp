#include "request.h"
#include <cstring>
#include <vector>

request::request():seq(-1),flag(0){}

request::request(uint32_t _seq):seq(_seq),par(_seq),flag(0){}

request::request(uint32_t _seq,uint32_t _par):seq(_seq),par(_par),flag(0){}

request::request(uint32_t _seq,const vector<bool>&ack):seq(_seq),flag(1){
    bs.reset();
    for(size_t i=0;i+_seq<ack.size() and i<BS;++i)
        bs[i]=ack[i+_seq];
}
