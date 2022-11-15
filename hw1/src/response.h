#include <string>
#include "error.h"

using namespace std;
using str=string;

class Response{
private:
    str server="llt mircd";
    str cname;
    const str header(const int&)const;
    void reply(const int&,const int&,const str&)const;
public:
    string message;
    Response(const str&);
    void reply(const int&,const RPL&)const;
    void reply(const int&,const ERR&)const;
};
