#include<string>

using namespace std;
using str=string;

class Response{
private:
    str server="llt mircd";
    str cname;
    str header;
public:
    int status;
    string message;
    Response(const str&);
    void reply(const int&fd,const str&msg)const;
};
