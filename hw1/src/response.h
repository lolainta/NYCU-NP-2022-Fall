#include<string>

using namespace std;
using str=string;

class Response{
private:
    const str header()const;
    str server="llt mircd";
    str cname;
public:
    int status;
    string message;
    Response(const str&);
    void reply();
};
