/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

using namespace std;


#define err_quit(m) { perror(m); exit(-1); }
#define cout cout<<"[Server] "

const unsigned int MAX_SIZE=32768;

typedef struct request{
    uint16_t seq;
    uint16_t checksum;
    request(int _seq):seq(_seq),checksum(_seq){};
}__attribute__((packed))request;

typedef struct{
    uint16_t seq;
    uint16_t size;
    uint32_t checksum;
    uint8_t filename[16];
    uint8_t file[MAX_SIZE];
}__attribute__((packed))response;

/*
void send_req(int sig){
    if(sig==SIGALRM){
        return;
        request req(seq);
        if(sendto(sock,&req,sizeof(req),0,(sockaddr*)&csin,sizeof(siin))<0)
            perror("sendto");
        alarm(1);
    }
}
*/
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
    cout<<"Server start running."<<endl;
    cout<<argv[0]<<' '<<argv[1]<<' '<<argv[2]<<' '<<argv[3]<<endl;
    if(argc<2)
        return -fprintf(stderr,"usage: %s ... <port>\n",argv[0]);
    assert(argc==4);

    get_ip();

    int sock;
    struct sockaddr_in sin;

    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stderr,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);

    cout<<sizeof(request)<<' '<<sizeof(response)<<endl;

    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    sin.sin_port=htons(strtol(argv[3],NULL,0));

    if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0)
        err_quit("socket");

    if(bind(sock,(sockaddr*)&sin,sizeof(sin))<0)
        err_quit("bind");

    string dir(argv[1]);
    std::filesystem::create_directories(dir);

//    signal(SIGALRM,send_req);
//    send_req(SIGALRM);

    int seq=0;
    response resp;
    for(int i=0;i<stoi(argv[2]);++i){
        sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        int rlen;
        
//        memset(&resp,0,sizeof(resp));
        cout<<"Waiting for "<<seq<<endl;
        if((rlen=recvfrom(sock,&resp,sizeof(response),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            cout<<rlen<<endl;
            break;
        }
        cout<<"Got: "<<resp.seq<<' '<<resp.filename<<endl;
        if(resp.seq==9999){
            --i;
            continue;
        }
        
        ofstream out(dir+'/'+string((char*)resp.filename),ios::out|ios::binary);
        out.write((char*)resp.file,resp.size);
        out.close();
        request req(++seq);
        sendto(sock,&req,sizeof(request),0,(sockaddr*)&csin,sizeof(csin));
        cout<<"ACK: "<<seq<<endl;
    }
    cout<<"Finished\n";

    close(sock);
}
