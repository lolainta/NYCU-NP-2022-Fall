#ifndef CLIENT_H
#define CLIENT_H
#include<string>

#include "response.h"

using namespace std;
using str=string;
using cstr=const string;

class Channel;

class Client{
private:
    Response resp;
    str nick,username;
    str hostname,servername,realname;
    cstr fixl(cstr&,const size_t&)const;
public:
    int id,fd;
    str ip,port;
    Channel*ch;
    Client(int);
    void reply(const RPL&)const;
    void reply(const RPL&,cstr&)const;
    void reply(const ERR&)const;
    void reply(const ERR&,cstr&)const;
    void reply(cstr&)const;
    void set_nick(cstr&);
    void set_name(cstr&,cstr&,cstr&,cstr&);
    void set_channel(Channel*);
    cstr get_nick()const;
    cstr info()const;
    cstr information()const;
    cstr connected()const;
    cstr disconnected();
    const bool notreg()const;
    bool reg()const;
};
#endif
