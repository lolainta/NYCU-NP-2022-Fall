#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

struct CNAME_t{
    std::vector<std::string> cname;
};

struct MX_t{
    uint16_t preference;
    std::vector<std::string> exchange;
};

struct NS_t{
    std::vector<std::string> nsdname;  
};

struct SOA_t{
    std::vector<std::string> mname;
    std::vector<std::string> rname;
    uint32_t serial;
    uint32_t refresh;
    uint32_t retry;
    uint32_t expire;
    uint32_t minumun;
};

struct TXT_t{
    uint8_t len;
    std::string txt_data;
};

struct A_t{
    uint8_t ip[4];
};

struct AAAA_t{
    uint8_t ip[16];
};

#endif
