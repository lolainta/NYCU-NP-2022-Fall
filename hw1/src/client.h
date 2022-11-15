#include<string>

#include "response.h"

using namespace std;
using str=string;

class Client{
private:
    Response resp;
    str nick,username;
    str hostname,servername,realname;
public:
    int id,fd;
    str ip,port;
    Client(int);
    void reply(const RPL&)const;
    void reply(const ERR&)const;
    void reply(const ERR&,const str&)const;
    void set_nick(const str&);
    void set_name(const str&,const str&,const str&,const str&);
    const str info()const;
    const str connected()const;
    const str disconnected();
    const bool notreg()const;
    void reg()const;
};
