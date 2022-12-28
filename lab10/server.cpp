#include <arpa/inet.h>
#include <linux/ip.h>
#include <iostream> 
#include <bitset>
#include <string>

using namespace std;

int main(int argc,char**argv){
    for(int i=0;i<argc;++i)
        cout<<argv[i]<<' ';
    cout<<endl;
    int sock;
    if((sock=socket(AF_INET,SOCK_RAW,161))<0){
        perror("socket");
        exit(1);
    }

    int sz;
    char buf[100];
    while((sz=recv(sock,buf,sizeof(buf),MSG_TRUNC))>0){
        cout<<sz<<endl;
        iphdr*iph=(iphdr*)buf;
        cout<<iph->version<<' '<<iph->ihl<<endl<<flush;
        string inp=string(buf+20);
        cout<<inp.size()<<' '<<inp<<endl;
    }

    return 0;
}
