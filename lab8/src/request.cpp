#include "request.h"
#include <cstring>

request::request():seq(-1),flag(0){}

request::request(uint32_t _seq):seq(_seq),flag(0){}

