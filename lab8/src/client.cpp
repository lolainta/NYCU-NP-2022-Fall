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

#define cout cout<<"[Client]\t"

int outCounter=0;
const int wrap=1000;
// const int wrap=1;

using str=std::string;
using cstr=const str;
using namespace std;

static int sock=-1;
static sockaddr_in sin;
static timeval curtv;
// static unsigned seq;
static uint32_t base;

const unsigned int MAX_DATA=32*1024*1024;

vector<response> pkts;

int frag(uint8_t*data,const unsigned int&size){
    const unsigned int rsize=(size/PAYLOAD+(bool)size%PAYLOAD)*PAYLOAD;
    assert(rsize%PAYLOAD==0);
    unsigned int num=rsize/PAYLOAD;
    assert(num<(1<<17)-1);
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

/*
void send_ping(int sig){
    unsigned char buf[1024];
    if(sig == SIGALRM) {
        ping_t *p = (ping_t*) buf;
        p->seq = seq++;
        gettimeofday(&p->tv, NULL);
        if(sendto(sock, p, sizeof(*p)+16, 0, (struct sockaddr*) &sin, sizeof(sin)) < 0)
            perror("sendto");
        alarm(1);
    }
    count++;
    if(count > 20) exit(0);
}
*/

void send_resp(const response&resp){
    sendto(sock,&resp,sizeof(response),0,(sockaddr*)&sin,sizeof(sin));
}

int send_cur(const size_t&wnd){
    cout<<"send base: "<<base<<endl;
    if(outCounter++%wrap==0)
        cout<<"Send start from "<<base<<" to "<<min(base+wnd,pkts.size())<<endl;
    if(base==pkts.size())
        return 1;
    for(unsigned i=base;i<base+wnd and i<pkts.size();++i)
       send_resp(pkts[i]);
    usleep(1000000);
    return 0;
}

void sender(int sig){
    while(send_cur(512)==0);
}

int main(int argc,char*argv[]) {
    assert(argc==5);
    cstr source(argv[1]);
//    const int fileNum(stoi(argv[2]));
    const int port(stoi(argv[3]));

    char*ip(argv[4]);

    cout<<sizeof(request)<<' '<<sizeof(response)<<endl;

    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    sin.sin_addr.s_addr=inet_addr(ip);

    if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket");
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
    cout<<num<<" packets generated"<<endl;

    uint8_t*syn=(uint8_t*)malloc(PAYLOAD);
    memcpy(syn,to_string(num+2).c_str(),PAYLOAD);
    pkts.insert(pkts.begin(),response(0,syn,0));
    pkts.front().flag=1;
    pkts.emplace_back(pkts.size(),syn,0);
    pkts.back().flag=2;

    thread th1(sender,0);

    while(1){
        int rlen;
        struct sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        request req;
        if((rlen=recvfrom(sock,&req,sizeof(req),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            continue;
        }
        if(!req.check()){
            cout<<"Request checksum failed!"<<endl;
            continue;
        }
        if(outCounter++%wrap==0)
            cout<<curTime()<<" received "<<req.seq<<'/'<<pkts.size()<<' '<<100.0*(req.seq-1)/pkts.size()<<'%'<<endl;
        base=max(base,req.seq);
        if(base==pkts.size()){
            cout<<"FINACK"<<endl;
            break;
        }
    }
    cout<<"End of while loop"<<endl;
    close(sock);
    cout<<"Client end"<<endl;
    th1.join();
    return 0;
}
