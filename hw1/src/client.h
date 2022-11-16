#ifndef CLIENT_H
#define CLIENT_H
#include<string>

#include "response.h"

using namespace std;
using str=string;

class Channel;

class Client{
private:
    Response resp;
    str nick,username;
    str hostname,servername,realname;
    vector<Channel>::iterator ch;
    const str fixl(const str&,const size_t&)const;
    const bool notreg()const;
public:
    int id,fd;
    str ip,port;
    Client(int);
    void reply(const RPL&)const;
    void reply(const RPL&,const str&)const;
    void reply(const ERR&)const;
    void reply(const ERR&,const str&)const;
    void reply(const str&)const;
    void set_nick(const str&);
    void set_name(const str&,const str&,const str&,const str&);
    void set_channel(vector<Channel>::iterator);
    const str info()const;
    const str information()const;
    const str connected()const;
    const str disconnected();
    bool reg()const;
};
#endif
