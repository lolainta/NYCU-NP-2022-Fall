#ifndef MESSAGE_H
#define MESSAGE_H

#include "enum.h"
#include "types.h"
#include <vector>
#include <string>

struct Header{
    uint16_t id;

    unsigned rd:1;
    unsigned tc:1;
    unsigned aa:1;
    unsigned opcode:4;
    unsigned qr:1;

    unsigned rcode:4;
    unsigned z:3;
    unsigned ra:1;
/*
    unsigned qr:1;
    unsigned opcode:4;
    unsigned aa:1;
    unsigned tc:1;
    unsigned rd:1;
    unsigned ra:1;
    unsigned z:3;
    unsigned rcode:4;
*/
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}__attribute__((packed));

struct Question{
    std::vector<std::string> qname;
    TYPE qtype;
    CLASS qclass;
    Question();
    Question(const Question&);
};

struct ResourceRecord{
    std::vector<std::string> rname;
    TYPE rtype;
    CLASS rclass;
    uint32_t ttl;
    uint16_t rdlength;
    union{
        struct A_t*a;
        struct AAAA_t*aaaa;
        struct CNAME_t*cname;
        struct NS_t*ns;
        struct SOA_t*soa;
        struct MX_t*mx;
        struct TXT_t*txt;
    };

    ResourceRecord();
    ResourceRecord(TYPE,uint32_t);
};

struct Message{
    struct Header hdr;
    std::vector<Question> ques;
    std::vector<ResourceRecord> ans;
    std::vector<ResourceRecord> auth;
    std::vector<ResourceRecord> add;
    Message();
    Message(size_t qd,size_t an,size_t ns,size_t ar);
    operator bool()const{
        return true;
    };
};

#endif
