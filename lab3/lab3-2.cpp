#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char const*argv[]){
	int sock=0,valread,client_fd;
	struct sockaddr_in serv_addr;

	if((sock=socket(AF_INET,SOCK_STREAM,0))<0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(10003);

	if(inet_pton(AF_INET,"140.113.213.213",&serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if((client_fd=connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)))<0){
		printf("\nConnection Failed \n");
		return -1;
	}

	char buffer[1048576]={0};
    string tmp;
	valread=read(sock,buffer,sizeof(buffer));
    string str=buffer;
    printf("[%d]\t%s\n",__LINE__,buffer);
    memset(buffer,0,sizeof(buffer));

    float arg=atof(argv[1]);
    cout<<arg<<endl;

    int speed=1000000*arg*0.985;
    char*trash=(char*)malloc(speed);
    for(int i=0;i<speed;++i)
        trash[i]='1';
    
    sleep(1);
    while(true){
        send(sock,trash,strlen(trash),0);
        sleep(1);
    }

    return 0;
}
