#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "request.h"
#include "response.h"

#define cout cout<<"[Server]\t"

using str=std::string;
using cstr=const str;
using namespace std;

int main(int argc,char*argv[]){
    assert(argc==4);
    str dest(argv[1]);
//    const int fileNum=stoi(argv[2]);
    const int port(stoi(argv[3]));

    cout<<sizeof(request)<<' '<<sizeof(response)<<endl;

    struct sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);

    int sock;
    if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket");
        exit(1);
    }

    if(bind(sock,(sockaddr*)&sin,sizeof(sin))<0){
        perror("error");
        exit(1);
    }

    vector<response> pkts;
    vector<bool> tmp;

    response*pkt=(response*)malloc(sizeof(response)+16);
    while(1){
        int rlen;
        sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        response&resp=*pkt;
        if((rlen=recvfrom(sock,&resp,sizeof(resp),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            exit(1);
        }
        if(!resp.check()){
            cout<<"Response checksum failed!"<<endl;
            continue;
        }
        cout<<"Recieved  "<<resp.seq<<endl;
        request req(resp.seq+1);
        if(resp.flag==1){
            cout<<"GOT SYN"<<endl;
            assert(resp.seq==0);
            pkts.assign(stoi((char*)resp.payload),response());
        }else if(resp.flag==2){
            cout<<"GOT FIN"<<endl;
            cout<<pkts.size()<<endl<<flush;
            cout<<resp.seq<<endl<<flush;
            assert(resp.seq==pkts.size()+1);
            break;
        }else{
            pkts[(int)resp.seq]=resp;
        }
        sendto(sock,&req,rlen,0,(sockaddr*)&csin,sizeof(csin));
    }

    cout<<"Sleeping..."<<endl;
    sleep(1);
    cout<<"Not close yet"<<endl;
//    close(sock);
    cout<<"Socket closed"<<endl;
    return 0;

}
