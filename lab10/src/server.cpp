#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

#include "fileIO.h"
#include "rawSocket.h"

#define cout cout<<"[Server]\t"

using namespace std;

using str=std::string;
using cstr=const str;

int main(int argc,char*argv[]){
    assert(argc==4);
    str dest(argv[1]);
    const size_t fileNum(stoi(argv[2]));
    char*ip(argv[3]);
    const int fragSize=1000;

    uint8_t*data=(uint8_t*)calloc(32*1024*1024,1);
    fileIO fio(data,0u);

    RawSocket sock;

    uint8_t*buf=(uint8_t*)calloc(fragSize,1);
    int acc=0;
    int cur=0;
    while((cur=sock.read(buf,fragSize))==fragSize){
        memcpy(data+acc,buf,cur);
        acc+=cur;
        memset(buf,0,fragSize);
    }
    free(buf);

    fio.writeFiles(dest);
    return 0;
}
