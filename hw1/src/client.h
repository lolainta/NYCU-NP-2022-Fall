#include<string>

using namespace std;
using str=string;

class Client{
public:
    int id,fd;
    str nick,name;
    str ip,port;
    Client(int);
    const str info()const;
};
