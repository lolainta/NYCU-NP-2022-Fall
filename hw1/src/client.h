#include<string>

#include "response.h"

using namespace std;
using str=string;

class Client{
private:
    Response resp;
public:
    int id,fd;
    str nick,username;
    str hostname,servername,realname;
    str ip,port;
    Client(int lisfd,const str&);
    void reply(const RPL&)const;
    void reply(const ERR&)const;
    const str info()const;
    const str connected()const;
    const str disconnected();
    const bool notreg()const;
};
