#include <iostream> 
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <unistd.h>
#include <string>

#define PROTOCOL 161
using namespace std;

iphdr*iph;

void fill(char*dst_ip,int len){
	iph->ihl=5;
    iph->version=4;
    iph->tos=0;
    iph->tot_len=sizeof(iphdr)+len;
    iph->id=htonl(54321);
    iph->frag_off=0;
    iph->ttl=255;
    iph->protocol=PROTOCOL;
    iph->check=0;
    iph->saddr=inet_addr("255.255.255.255");
    iph->daddr=inet_addr("dst_ip");
}

int main(int argc,char**argv){
    for(int i=0;i<argc;++i)
        cout<<argv[i]<<' ';
    cout<<endl;
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
    dst.sin_addr.s_addr=inet_addr(argv[3]);

    if(connect(sock,(sockaddr*)&dst,sizeof(dst))<0){
        perror("connect");
        exit(1);
    }

    char buf[1000];
    string msg="abc123";
    iph=(iphdr*)buf;
    memcpy(buf+20,msg.c_str(),msg.size());
    fill(argv[3],msg.size());
    send(sock,buf,sizeof(buf),0);
    cout<<"hello"<<endl;

    return 0;
}
