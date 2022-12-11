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

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "request.h"
#include "response.h"
#include "fileIO.h"

#define cout cout<<"[Client]\t"

using str=std::string;
using cstr=const str;
using namespace std;

static int sock=-1;
static sockaddr_in sin;
static timeval curtv;
// static unsigned seq;
static uint16_t cur;

const unsigned int RAW_DATA=33*1024*1024;
const unsigned int DATA=(RAW_DATA/PAYLOAD+(bool)RAW_DATA%PAYLOAD)*800;

vector<response> pkts;

void frag(uint8_t*data){
    vector<response>&ret=pkts;
    uint16_t pid=1;
    unsigned int num=DATA/PAYLOAD;
    assert(DATA%PAYLOAD==0);
    for(unsigned i=0;i<num;++i)
        ret.emplace_back(pid++,data,i);
    assert(pkts.size()==(RAW_DATA/PAYLOAD+(bool)RAW_DATA%PAYLOAD));
    for(int i=0;i<(int)pkts.size();++i)
        assert(pkts[i].seq==1+i);
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

void send_cur(){
    if(cur<pkts.size()){
        cout<<"Send "<<cur<<endl;
        send_resp(pkts[cur]);
        return;
    }else{
        cout<<"`cur` too large"<<endl;
    }
}

void alrm(int sig){
    send_cur();
    alarm(1);
    
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

    uint8_t*data=(uint8_t*)calloc(DATA,1);
    fileIO fio(data,DATA);
    fio.readFiles(source);
    frag(data);

    uint8_t*syn=(uint8_t*)malloc(1000);
    cout<<"PKT size: "<<pkts.size()<<endl;
    memcpy(syn,to_string(pkts.size()).c_str(),1000);
    pkts.insert(pkts.begin(),response(0,syn,0));
    pkts.front().flag=1;
    pkts.emplace_back(pkts.size(),syn,0);
    pkts.back().flag=2;

    signal(SIGALRM,alrm);
    alrm(SIGALRM);

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
        cout<<curTime()<<" received "<<req.seq<<endl;
        cur=max(cur,req.seq);
    }

    close(sock);
}
