#include "request.h"
#include <cstring>

request::request():seq(16383),flag(0){}

request::request(uint16_t _seq):seq(_seq),flag(0){
    checksum=genHash();
}

uint16_t request::genHash(){
    return seq;
}

bool request::check(){
    return genHash()==checksum;
}

