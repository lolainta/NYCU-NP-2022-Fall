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
// const int wrap=1;
const int wrap=6001;

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

    response resp;
    while(1){
        int rlen;
        sockaddr_in csin;
        socklen_t csinlen=sizeof(csin);
        if((rlen=recvfrom(sock,&resp,sizeof(resp),0,(sockaddr*)&csin,&csinlen))<0){
            perror("recvfrom");
            exit(1);
        }
        uint32_t cur=resp.seq;
        if(outCounter++%wrap==0)
            cout<<"Recieved "<<cur<<endl;
        if(resp.flag==1){
            cout<<"GOT SYN"<<endl;
            assert(resp.seq==0);
            size_t size=stoi((char*)resp.payload);
            cout<<"Number of packets: "<<size<<endl;
            pkts.resize(size,response());
            ack.resize(size,false);
        }else if(resp.flag==2 and !pkts.empty()){
            cout<<"GOT FIN"<<endl;
            assert(resp.seq==pkts.size()-1);
        }else{
            if(base==0 and resp.seq!=0)
                continue;
            pkts[cur]=resp;
        }
        request req;
        ack[cur]=true;
        if(cur==base){
            while(base<pkts.size() and ack[base])
                base++;
            if(outCounter++%wrap==0)
                cout<<"Set base to "<<base<<endl;
            req=request(base);
        }else{
            if(resp.seq>=base+BS)
                req=request(base,cur);
            else
                req=request(base,ack);
        }
        if(base!=pkts.size())
            assert(ack[base]==false);
        if(outCounter++%wrap==0){
            cout<<"Send: "<<req.seq<<endl;
            int load=0;
            for(auto b:ack)
                if(b)
                    ++load;
            cout<<load<<' '<<ack.size()<<' '<<100.0*load/ack.size()<<'%'<<endl;
        }
        for(int i=0;i<10;++i)
            sendto(sock,&req,rlen,0,(sockaddr*)&csin,sizeof(csin));
        if(base==pkts.size()){
            cout<<"Received last packet."<<endl;
            for(int i=0;i<10;++i)
                sendto(sock,&req,rlen,0,(sockaddr*)&csin,sizeof(csin));
            break;
        }
            
    }
    cout<<"End of while loop"<<endl;

    uint8_t*data=(uint8_t*)calloc(DATA,1);
    defrag(data);
    cout<<"Finish defragment"<<endl;
    fileIO fio(data,DATA);
    fio.writeFiles(dest);
    cout<<"File wrote"<<endl;
    close(sock);
    cout<<"Server end"<<endl;
    return 0;

}
