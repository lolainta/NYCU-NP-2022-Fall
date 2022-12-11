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
#include "fileIO.h"

#define cout cout<<"[Server]\t"
int outCounter=0;
const int wrap=1000;

using str=std::string;
using cstr=const str;
using namespace std;

const unsigned int RAW_DATA=32*1024*1024;
const unsigned int DATA=(RAW_DATA/PAYLOAD+(bool)RAW_DATA%PAYLOAD)*PAYLOAD;

vector<response> pkts;
vector<bool> ack;
size_t base=0;

void defrag(uint8_t*data){
    for(size_t i=0;i+2<=pkts.size();++i){
        memcpy(data+i*PAYLOAD,pkts[i+1].payload,PAYLOAD);
    }
    cout<<"Finish defragment"<<endl;
}

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
        uint32_t cur=resp.seq;
        if(outCounter++%wrap==0)
            cout<<"Recieved "<<cur<<endl;
        if(resp.flag==1){
            cout<<"GOT SYN"<<endl;
            assert(resp.seq==0);
            size_t size=stoi((char*)resp.payload);
            cout<<"Number of packets: "<<size<<endl;
            pkts.assign(size,response());
            ack.assign(size,false);
        }else if(resp.flag==2){
            cout<<"GOT FIN"<<endl;
            cout<<pkts.size()<<endl<<flush;
            cout<<resp.seq<<endl<<flush;
            assert(resp.seq==pkts.size()+1);
            break;
        }else{
            if(base==0 and resp.seq!=0)
                continue;
            pkts[cur]=resp;
        }
//        assert(cur>=base);
        ack[cur]=true;
        if(cur==base){
            while(base+1<=pkts.size() and ack[++base]);
            if(outCounter++%wrap==0)
                cout<<"Set base to "<<base<<endl;
        }
        if(base*2>=pkts.size()){
            cout<<"Debug mode"<<endl;
            break;
        }
        if(base==pkts.size()){
            cout<<"Received last packet."<<endl;
            request req(base);
            sendto(sock,&req,rlen,0,(sockaddr*)&csin,sizeof(csin));
            break;
        }
        request req(base);
        assert(ack[base]==false);
        if(outCounter++%wrap==0)
            cout<<"Send: "<<base<<endl;
        sendto(sock,&req,rlen,0,(sockaddr*)&csin,sizeof(csin));
    }
    cout<<"End of while loop."<<endl;
    
    uint8_t*data=(uint8_t*)calloc(DATA,1);
    defrag(data);
    fileIO fio(data,DATA);
    fio.writeFiles(dest);
    close(sock);
    return 0;

}
