/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
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

#include <net/if.h>
#include <sysexits.h>
#include <ifaddrs.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cassert>

using namespace std;

#define err_quit(m) { perror(m); exit(-1); }

#define NIPQUAD(s)    ((unsigned char *) &s)[0], \
                    ((unsigned char *) &s)[1], \
                    ((unsigned char *) &s)[2], \
                    ((unsigned char *) &s)[3]

#define cout cout<<"[Client] "

static int sock=-1;
static struct sockaddr_in sin;
static uint16_t seq=0;
static unsigned timeout_t=0;

const unsigned int MAX_SIZE=32768;

typedef struct{
    uint16_t seq;
    uint16_t checksum;
}__attribute__((packed))request;

typedef struct response{
    uint16_t seq;
    uint16_t size;
    uint32_t checksum;
    uint8_t filename[16];
    uint8_t file[MAX_SIZE];
    response(uint16_t _seq,uint16_t _size,uint8_t*_file,string _fname):seq(_seq),size(_size){
        strncpy((char*)file,(char*)_file,size);
        strcpy((char*)filename,_fname.c_str());
        checksum=0;
    }
}__attribute__((packed))response;

vector<response> files;
void do_send(int sig){
    if(sig==SIGALRM){
        cout<<"Send file: "<<seq<<endl;
        if(sendto(sock,files.data()+seq,sizeof(response)+16,0,(sockaddr*)&sin,sizeof(sin))<0)
            perror("sendto");
        alarm(1);
    }
    timeout_t++;
    cout<<"timeout_t: "<<timeout_t<<endl;
    if(timeout_t>10)
        exit(0);
}

char ip[32];
void get_ip(){
    ifaddrs*ifs=nullptr;
    getifaddrs(&ifs);
    for(auto ifen=ifs;ifen!=nullptr;ifen=ifen->ifa_next){
        sa_family_t addr_fam=ifen->ifa_addr->sa_family;
        if(addr_fam==AF_INET and ifen->ifa_addr!=nullptr){
            inet_ntop(addr_fam,&((sockaddr_in*)ifen->ifa_addr)->sin_addr,ip,32);
            if(string(ip).substr(0,4)=="172."){
                cout<<"IP: "<<ip<<endl;
                break;
            }
        }
    }
}
int main(int argc,char*argv[]){
    cout<<"Client start running."<<endl<<flush;
    cout<<argv[0]<<' '<<argv[1]<<' '<<argv[2]<<' '<<argv[3]<<' '<<argv[4]<<endl;
    if(argc<3)
        return -fprintf(stderr,"usage: %s ... <port> <ip>\n",argv[0]);
    assert(argc==5);

    get_ip();

    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stderr,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);

    cout<<sizeof(request)<<' '<<sizeof(response)<<endl;

    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(strtol(argv[3],NULL,0));
//    if(inet_pton(AF_INET,argv[4],&sin.sin_addr)!=1)
    if(inet_pton(AF_INET,ip,&sin.sin_addr)!=1)
        return -fprintf(stderr,"** cannot convert IPv4 address for %s\n",argv[argc-1]);
//    sin.sin_addr.s_addr=inet_addr(ip);

    if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0)
        err_quit("socket");
    printf("[Client] %u.%u.%u.%u/%u\n",NIPQUAD(sin.sin_addr), ntohs(sin.sin_port));

    string dir(argv[1]);
    uint8_t buffer[MAX_SIZE];
    uint16_t cnt=0;
    for(const auto&file:filesystem::directory_iterator(dir)){
        memset(buffer,0,sizeof(buffer));
        ifstream in(file.path(),ios::in|ios::binary);
        unsigned int sz=file_size(file.path());
        assert(sz<MAX_SIZE);
        if(in.fail())
            perror("ifstream");
        in.read((char*)buffer,sz);
        files.emplace_back(cnt++,sz,buffer,file.path().filename());
        in.close();
    }
    cout<<"File stored"<<endl;
    assert(stoi(argv[2])==(int)files.size());
    for(int i=0;i<stoi(argv[2]);++i)
        assert(files[i].seq==i);

    char tmp[10]="abc123\n";
    response syn(9999,8,(uint8_t*)tmp,"SYN");
    for(int i=0;i<5;++i)
        sendto(sock,&syn,sizeof(response),0,(sockaddr*)&sin,sizeof(sin));

    signal(SIGALRM,do_send);
    do_send(SIGALRM);

    while(1){
        int rlen;
        struct sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        request req;
        if((rlen=recvfrom(sock,&req,sizeof(req),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            continue;
        }
        timeout_t=0;
        if(req.seq!=req.checksum){
            cout<<"Checksum mismatch!\n"<<endl;
            continue;
        }
        cout<<"ACK: "<<req.seq<<endl;
        if(req.seq==stoi(argv[2])){
            cout<<"Last ack got! finished\n";
            exit(0);
        }

        seq=max(seq,req.seq);
        sendto(sock,&files[seq],sizeof(response),0,(sockaddr*)&csin,sizeof(csin));
    }
    close(sock);
}
