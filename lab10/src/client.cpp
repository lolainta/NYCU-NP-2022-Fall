#include <iostream>
#include <cassert>
#include <string>
#include <cstring>
#include <thread>

#include "fileIO.h"
#include "rawSocket.h"
#include "response.h"
#include "request.h"


#define cout cout<<"[Client]\t"
int outCounter=0;
const int wrap=4001;

using namespace std;

using str=std::string;
using cstr=const str;

const unsigned int MAX_DATA=32*1024*1024;

RawSocket rSock(0,162);
RawSocket sSock(1,161);
vector<response> pkts;
vector<bool> ack;
static uint32_t base;


int frag(uint8_t*data,const unsigned int&size){
    const unsigned int rsize=(size/PAYLOAD+(bool)size%PAYLOAD)*PAYLOAD;
    assert(rsize%PAYLOAD==0);
    unsigned int num=rsize/PAYLOAD;
    vector<response>&ret=pkts;
    uint32_t pid=1;
    for(unsigned i=0;i<num;++i)
        ret.emplace_back(pid++,data,i);
    for(int i=0;i<(int)pkts.size();++i)
        assert((int)pkts[i].seq==1+i);
    return pkts.size();
}

void send_resp(const response&resp){
    sSock.send((uint8_t*)&resp,sizeof(response));
}

int send_cur(const size_t&wnd,size_t offset){
    size_t st=base;
    size_t cnt=0;
    if(outCounter++%wrap==0)
        cout<<"send base: "<<base<<endl;
    if(outCounter++%wrap==0)
        cout<<"Send start from "<<base<<" to "<<min(base+wnd,pkts.size())<<endl;
    if(base==pkts.size())
        return 1;
    for(size_t i=st;i<pkts.size();++i){
        if(!ack[i])
            send_resp(pkts[i]),++cnt;
        if(base==0 and i>=1000)
            return 0;\
        if(cnt>wnd){
            usleep(1000);
            if(outCounter++%wrap==0)
                cout<<"Sleep when sending "<<i<<endl;
            cnt-=wnd;
            continue;
            return 0;
        }
    }
    usleep(1000);
    return 0;
}

void sender(int sz,int offset){
    while(send_cur(sz,offset)==0);
}


int main(int argc,char*argv[]){
    assert(argc==4);
    str source(argv[1]);
//    const size_t fileNum(stoi(argv[2]));
    char*ip(argv[3]);
    sSock.set_iphdr(ip);

    cout<<"Allocate memory for fileIO"<<endl;
    uint8_t*data=(uint8_t*)calloc(MAX_DATA,1);
    cout<<"Start fileIO"<<endl;
    fileIO fio(data,MAX_DATA);
    int total=fio.readFiles(source);
    cout<<total<<" bytes in total"<<endl;
    cout<<"FileIO end"<<endl;
    int num=frag(data,total);
    ack.assign(num+2,false);
    cout<<num<<" packets generated"<<endl;

    uint8_t*syn=(uint8_t*)malloc(PAYLOAD);
    memcpy(syn,to_string(num+2).c_str(),PAYLOAD);
    pkts.insert(pkts.begin(),response(0,syn,0));
    pkts.front().flag=1;
    pkts.emplace_back(pkts.size(),syn,0);
    pkts.back().flag=2;

    cout<<ack.size()<<' '<<pkts.size()<<endl;

    vector<thread> thrds;
    thrds.emplace_back(sender,BS,0);

    request req;
    while(1){
        rSock.read((uint8_t*)&req,sizeof(request));
        assert(req.seq==ack.size() or ack[req.seq]==false);
        for(size_t i=base;i<req.seq;++i)
            ack[i]=true;
        if(req.flag==1){
            for(int i=0;i<BS and i+req.seq<ack.size();++i){
//                assert(!ack[req.seq+i] or req.bs[i]);
                if(ack[req.seq+i]==true and req.bs[i]==false){
                    cout<<req.seq<<' '<<i<<endl;
                }
                ack[req.seq+i]=(bool)req.bs[i];
            }
        }else if(req.par!=req.seq){
            ack[req.par]=true;
        }

        if(outCounter++%wrap==0){
            int load=0;
            for(auto b:ack)
                if(b)
                    ++load;
            // cout<<curTime()<<" received "<<req.par<<' '<<req.seq<<' '<<load<<'/'<<pkts.size()<<' '<<100.0*load/pkts.size()<<'%'<<endl;
        }
        base=max(base,req.seq);
        if(base==pkts.size()){
            cout<<"FINACK"<<endl;
            break;
        }
    }
    cout<<"End of while loop"<<endl;

    for(auto&t:thrds)
        t.join();
    cout<<"Client end"<<endl;
    sleep(1);
    return 0;
}