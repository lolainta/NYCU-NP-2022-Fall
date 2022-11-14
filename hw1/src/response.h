#include<string>

using namespace std;
using str=string;

class Response{
private:
    str server="llt mircd";
    str cname;
    const str header(const int&)const;
public:
    string message;
    Response(const str&);
    void reply(const int&,const int&,const str&)const;
};
