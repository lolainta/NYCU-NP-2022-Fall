#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

#include "fileIO.h"
#include "rawSocket.h"

#define cout cout<<"[Client]\t"

using namespace std;

using str=std::string;
using cstr=const str;

int main(int argc,char*argv[]){
    assert(argc==4);
    str source(argv[1]);
    const size_t fileNum(stoi(argv[2]));
    char*ip(argv[3]);
    const int fragSize=1000;

    uint8_t*data=(uint8_t*)calloc(32*1024*1024,1);

    fileIO fio(data,0u);
    int tolSize=fio.readFiles(source);
    cout<<tolSize<<" bytes need to be sent"<<endl;

    RawSocket sock;
    sock.set_iphdr(ip,fragSize);

    uint8_t*buf=(uint8_t*)calloc(1000,sizeof(uint8_t));
    for(int i=0;i<tolSize/fragSize;++i){
        memcpy(buf,data+i*fragSize,fragSize);
        sock.send(buf,fragSize);
        memset(buf,0,fragSize);
        sleep(1);
    }
    if(int remain=tolSize%fragSize){
        cout<<"remain "<<remain<<endl;
        memcpy(buf,data+tolSize/fragSize*fragSize,remain);
        sock.send(buf,remain);
    }
    return 0;

}
