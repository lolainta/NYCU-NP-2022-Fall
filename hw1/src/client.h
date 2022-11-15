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
    void reply(const int&)const;
    void reply(const str&)const;
    const str info()const;
};
