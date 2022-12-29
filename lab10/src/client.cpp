#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <thread>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "request.h"
#include "response.h"
#include "fileIO.h"
#include "ip.cpp"

#define cout cout<<"[Client]\t"

int outCounter=0;
const int wrap=4001;
// const int wrap=1;

using str=std::string;
using cstr=const str;
using namespace std;

static int sock=-1;
static sockaddr_in sin;
static timeval curtv;
static char*dst_ip;
// static unsigned seq;
static uint32_t base;

const unsigned int MAX_DATA=32*1024*1024;

vector<response> pkts;
vector<bool> ack;

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

typedef struct{
    unsigned seq;
    struct timeval tv;
}ping_t;

double tv2ms(timeval*tv){
    return 1000.0*tv->tv_sec+0.001*tv->tv_usec;
}

cstr curTime(){
    gettimeofday(&curtv,nullptr);
    return to_string(tv2ms(&curtv));
}

void send_resp(const response&resp){
    uint8_t*buf=(uint8_t*)calloc(sizeof(response),1);
    iphdr*iph=(iphdr*)buf;
    fill(iph,dst_ip,sizeof(response));
    memcpy(buf+20,&resp,sizeof(resp));
    send(sock,buf,sizeof(buf),0);
    free(buf);
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
            return 0;
        if(cnt>wnd){
            if(outCounter++%wrap==0)
                cout<<"Sleep when sending "<<i<<endl;
            usleep(90);
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

int main(int argc,char*argv[]) {
    assert(argc==4);
    cstr source(argv[1]);
//    const int fileNum(stoi(argv[2]));
    char*ip(argv[3]);
    dst_ip=ip;

    int sock;
    if((sock=socket(AF_INET,SOCK_RAW,PROTOCOL))<0){
        perror("socket");
        exit(1);
    }
    int yes=1;
    setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&yes,sizeof(yes));
    setsockopt(sock,IPPROTO_IP,IP_HDRINCL,&yes,sizeof(yes));

    struct sockaddr_in dst;
    bzero(&dst,sizeof(dst));
    dst.sin_family=AF_INET;
    dst.sin_addr.s_addr=inet_addr(ip);

    if(connect(sock,(sockaddr*)&dst,sizeof(dst))<0){
        perror("connect");
        exit(1);
    }

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

    vector<thread> thrds;
    thrds.emplace_back(sender,BS,0);

    while(1){
        int rlen;
        struct sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        request req;
        if((rlen=recvfrom(sock,&req,sizeof(req),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            continue;
        }
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
            cout<<curTime()<<" received "<<req.par<<' '<<req.seq<<' '<<load<<'/'<<pkts.size()<<' '<<100.0*load/pkts.size()<<'%'<<endl;
        }
        base=max(base,req.seq);
        if(base==pkts.size()){
            cout<<"FINACK"<<endl;
            break;
        }

    }
    cout<<"End of while loop"<<endl;
    close(sock);
    cout<<"Client end"<<endl;
    for(auto&t:thrds)
        t.join();
    return 0;
}
