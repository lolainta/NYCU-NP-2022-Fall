#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

#include "fileIO.h"
#include "rawSocket.h"
#include "response.h"
#include "request.h"

#define cout cout<<"[Server]\t"

int outCounter=0;
// const int wrap=1;
const int wrap=6001;
using namespace std;

using str=std::string;
using cstr=const str;


const unsigned int RAW_DATA=32*1024*1024;
const unsigned int DATA=(RAW_DATA/PAYLOAD+(bool)RAW_DATA%PAYLOAD)*PAYLOAD;

void defrag(const vector<response>&pkts,uint8_t*data){
    for(size_t i=0;i+2<=pkts.size();++i){
        memcpy(data+i*PAYLOAD,pkts[i+1].payload,PAYLOAD);
    }
}

int main(int argc,char*argv[]){
    assert(argc==4);
    str dest(argv[1]);
//    const size_t fileNum(stoi(argv[2]));
    char*ip(argv[3]);

    cout<<sizeof(request)<<' '<<sizeof(response)<<endl;

    RawSocket rSock(0,161);
    RawSocket sSock(1,162);
    sSock.set_iphdr(ip);

    uint8_t test[100];
    memset(test,0,sizeof(test));

    vector<response> pkts;
    vector<bool> ack;
    size_t base=0;

    response resp;
    while(1){
        rSock.read((uint8_t*)&resp,sizeof(response));
        uint32_t cur=resp.seq;
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
            sSock.send((uint8_t*)&req,sizeof(request));
        if(base==pkts.size()){
            cout<<"Received last packet."<<endl;
            for(int i=0;i<10;++i)
                sSock.send((uint8_t*)&req,sizeof(request));
            break;
        }
            
    }
    cout<<"End of while loop"<<endl;

    uint8_t*data=(uint8_t*)calloc(DATA,1);
    defrag(pkts,data);
    cout<<"Finish defragment"<<endl;
    fileIO fio(data,DATA);
    fio.writeFiles(dest);
    cout<<"Server end"<<endl;
    return 0;
}
