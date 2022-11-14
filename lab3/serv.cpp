#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 10000

using namespace std;

int main(int argc,char**argv){
    int listenfd,connfd;
    socklen_t len;
    struct sockaddr_in servaddr,cliaddr;
    char buff[MAXLINE];
    time_t ticks;

    listenfd=socket(AF_INET,SOCK_STREAM,0);
   
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=Htonl(INADDR_ANY);
    servaddr.sin_port=htons(13);
    cout<<"Hello, world!"<<endl;
    return 0;
}
