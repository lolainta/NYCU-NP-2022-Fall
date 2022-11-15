#include <string>
#include "error.h"

using namespace std;
using str=string;

class Response{
private:
    str cname;
    void reply(const str&)const;
public:
    int fd;
    str server="llt-mircd";
    const str header(const int&)const;
    void reply(const vector<str>&)const;
    void set_cname(const str&);
};
