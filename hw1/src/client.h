#include<string>

#include "response.h"

using namespace std;
using str=string;

class Client{
public:
    int id,fd;
    str nick,name;
    str ip,port;
    Response resp;
    Client(int,const str&);
    void reply(const str&);
    const str info()const;
};
