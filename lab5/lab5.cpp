#include<iostream>
#include<vector>
#include<cassert>
#include<string>
#include<string.h>
#include<signal.h>
#include<memory>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
//#include<libexplain/read.h>

#define memset(...)

using namespace std;
using str=string;

typedef struct client{
    int id,fd;
    str name;
    str ip,port;
    client(int _id){
        id=_id;
        name="";
        for(int i=0;i<19;++i){
            name+=(char)(rand()%26+'a');
        }
    }
}Client;

void announce(const vector<Client>&clis,str msg,int id){
    msg+='\0';
    char*buf=(char*)malloc(1024);
    memset(buf,0,sizeof(buf));
    strncpy(buf,msg.c_str(),msg.size());
	for(const auto&x:clis)
        if(x.id!=id and x.fd!=-1)
            write(x.fd,buf,msg.size());
    free(buf);
    return;
}

const str disconnect(Client&x){
    str ret="* client "+x.ip+":"+x.port+" disconnected\n";
    return ret;
}

const str connect(Client&x){
    str ret="* client connected from "+x.ip+":"+x.port+"\n";
    return ret;
}

const str time(){
    time_t now=time(0);
    struct tm tstruct;
    char buf[128];
    tstruct=*localtime(&now);
    strftime(buf,sizeof(buf),"%Y-%m-%d %X ",&tstruct);
    return buf;
}

const str prompt(const str&role){
    if(role=="\0")
        return time()+"*** ";
    return time()+"<"+role+"> ";
}

const str banner(int size,str name){
	str ret=prompt("\0")+"Welcome to the simple CHAT server\n";
	ret+=prompt("\0")+"Total "+to_string(size)+" users online now. Your name is <"+name+">\n";
	return ret;
}

const str who(const vector<Client> clis,int id){
    str ret;
    ret+="--------------------------------------------------\n";
    for(auto&cli:clis)
        ret+=(id==cli.id?"* ":"  ")+cli.name+"\t\t"+cli.ip+":"+cli.port+"\n";
    ret+="--------------------------------------------------\n";
    return ret;
}

const str online(const str&name){
    str ret;
    ret+="User <"+name+"> has just landed on the server\n";
    return ret;
}

const str offline(const str&name){
    str ret;
    ret+="User <"+name+"> has left the server\n";
    return ret;
}

const str change(const str&org,const str&nxt,bool user=false){
    str ret;
    if(!user)
        ret+="User <"+org+"> renamed to <"+nxt+">\n";
    else
        ret+="Nickname changed to <"+nxt+">\n";
    return ret;
}

const str nocmd(const str&err){
    str ret;
    ret+="Unknown or incomplete command <"+err+">\n";
    return ret;
}

const str text(const str&sender,const str&msg){
    str ret;
    ret+=prompt(sender)+msg;
    return ret;
}

int main(int argc,char**argv){
    int server_port=stoi(argv[1]);
    struct sockaddr_in servaddr,cliaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(listenfd,4096);

    vector<Client> clients;
    
    int cnt=0;
    const int buf_size=65536;
    char read_buf[buf_size];
    char write_buf[buf_size];
    signal(SIGPIPE,SIG_IGN);
    while(1){
        memset(read_buf,0,buf_size);
        memset(write_buf,0,buf_size);
        int maxfd=listenfd;
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(listenfd,&rset);
        for(auto x:clients)
            if(x.fd!=-1)
                maxfd=max(maxfd,x.fd),FD_SET(x.fd,&rset);
            
        int nready=select(maxfd+1,&rset,nullptr,nullptr,nullptr);
        assert(nready);
        if(FD_ISSET(listenfd,&rset)){
            Client cur(cnt++);
            struct sockaddr_in cliaddr;
            socklen_t clilen=sizeof(cliaddr);
            cur.fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
            cur.port=to_string(htons(cliaddr.sin_port));
            cur.ip=(inet_ntoa(cliaddr.sin_addr));
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);

            cout<<connect(cur);

            str tmp=banner(clients.size(),cur.name);
            strcpy(write_buf,tmp.c_str());
            write(cur.fd,write_buf,tmp.size());

            str announcement=prompt("\0")+online(cur.name);
            announce(clients,announcement,cur.id);
            continue;
        }
        for(auto&cli:clients){
            if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
                int readn=read(cli.fd,read_buf,buf_size);
                if(readn<=0 or read_buf[0]=='\0'){
                    if(read_buf[0]=='\0')
                        readn=read(cli.fd,read_buf,1);
                    memset(read_buf,0,buf_size);
                    cout<<disconnect(cli);
                    str msg=prompt("\0")+offline(cli.name);
                    announce(clients,msg,cli.id);
                    assert(cli.fd!=listenfd);
                    close(cli.fd);
                    cli.fd=-1;
                }else{
                    if(read_buf[0]=='/'){
                        if(!strncmp(read_buf,"/who ",5) or !strncmp(read_buf,"/who\n",5)){
                            str msg;
                            msg=who(clients,cli.id);
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }else if(!strncmp(read_buf,"/name ",6)){
                            str org=cli.name;
                            cli.name="";
                            char nxt[128];
                            strcpy(nxt,read_buf+6);
                            char*tmp=nxt;
                            while(1){
                                if(*tmp=='\n')
                                    break;
                                cli.name+=*tmp++;
                            }
                            str msg;
                            msg=prompt("\0")+change(org,cli.name,true);
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());

                            msg=prompt("\0")+change(org,cli.name,false);
                            strcpy(write_buf,msg.c_str());
                            announce(clients,msg,cli.id);
                        }else{
                            str msg;
                            str err(read_buf);
                            for(int i=0;i<err.size();++i)
                                if(err[i]=='\n')
                                    err.erase(err.begin()+i,err.end());
                            msg=prompt("\0")+nocmd(err);
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }
                    }else{
                        assert(read_buf[0]!='\0');
                        str message(read_buf);
                        for(int i=0;i<message.size();++i)
                            if(message[i]=='\n')
                                message.erase(message.begin()+i+1,message.end());
                                
                        str msg=text(cli.name,message);
                        strcpy(write_buf,msg.c_str());
                        announce(clients,msg,cli.id);
                    }
                }
            }
        }

        for(int i=clients.size()-1;i>=0;--i)
            if(clients[i].fd==-1){
                assert(i<clients.size());
                clients.erase(clients.begin()+i);
            }
    }
    return 0;
}
