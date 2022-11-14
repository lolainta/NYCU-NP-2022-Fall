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
#include<chrono>
//#include<libexplain/read.h>

#define memset(...)

using namespace std;
using str=string;

typedef struct client{
    int id,fd;
    str name;
    str ip,port;
    bool cmd=true;
    client(int _id,bool _cmd=true){
        id=_id;
        name="";
        for(int i=0;i<19;++i)
            name+=(char)(rand()%26+'a');
        cmd=_cmd;
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

int counter=0;

const uint32_t gettime(){
    using namespace std::chrono;
    return static_cast<uint32_t>(duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count());
}
int last=0;
const str reset(){
    last=gettime();
    str ret=to_string((int)gettime())+" RESET "+to_string(counter)+"\n";
    counter=0;
    return ret;
}
const str ping(){
    return to_string((int)gettime())+" PONG\n";
}

const str report(){
    auto cur=gettime();
    str ret=to_string(gettime())+" REPORT "+to_string(counter)+' ';
    auto interval=cur-last;
    ret+=to_string(interval/1000)+"s ";
    ret+=to_string((long double)counter/(long double)interval);
    ret+="Mpbs\n";
    last=cur;
    return ret;
}

const str client(const vector<Client> clis){
    int num=0;
    for(auto c:clis)
        if(!c.cmd)
            num++;
    return to_string(gettime())+" CLIENTS "+to_string(num)+"\n";
}

int main(int argc,char**argv){
    int server_port=stoi(argv[1]);
    int sink_port=server_port+1;
    struct sockaddr_in servaddr,sinkaddr,cliaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    int listenfd_sink=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(listenfd,4096);
    sinkaddr.sin_family=AF_INET;
    sinkaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sinkaddr.sin_port=htons(sink_port);
    bind(listenfd_sink,(struct sockaddr*)&sinkaddr,sizeof(sinkaddr));
    listen(listenfd_sink,4096);

    vector<Client> clients;
    
    last=gettime();
    int cnt=0;
    const int buf_size=65536;
    char sink_buf[buf_size];
    char read_buf[buf_size];
    char write_buf[buf_size];
    signal(SIGPIPE,SIG_IGN);
    while(1){
        memset(sink_buf,0,buf_size);
        memset(read_buf,0,buf_size);
        memset(write_buf,0,buf_size);
        int maxfd=max(listenfd,listenfd_sink);
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(listenfd,&rset);
        FD_SET(listenfd_sink,&rset);
        for(auto x:clients)
            if(x.fd!=-1)
                maxfd=max(maxfd,x.fd),FD_SET(x.fd,&rset);
            
        int nready=select(maxfd+1,&rset,nullptr,nullptr,nullptr);
        assert(nready);
        if(FD_ISSET(listenfd,&rset)){
            Client cur(cnt++,true);
            struct sockaddr_in cliaddr;
            socklen_t clilen=sizeof(cliaddr);
            cur.fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
            cur.port=to_string(htons(cliaddr.sin_port));
            cur.ip=(inet_ntoa(cliaddr.sin_addr));
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);

            cout<<connect(cur);
            continue;
        }
        if(FD_ISSET(listenfd_sink,&rset)){
            Client cur(cnt++,false);
            struct sockaddr_in cliaddr;
            socklen_t clilen=sizeof(cliaddr);
            cur.fd=accept(listenfd_sink,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
            cur.port=to_string(htons(cliaddr.sin_port));
            cur.ip=(inet_ntoa(cliaddr.sin_addr));
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);
        }
        for(auto&cli:clients){
            if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
                int readn=read(cli.fd,read_buf,buf_size);
                if(readn<=0 or read_buf[0]=='\0'){
                    if(read_buf[0]=='\0')
                        readn=read(cli.fd,read_buf,1);
                    memset(read_buf,0,buf_size);
                    cout<<disconnect(cli);
                    /*
                    str msg=prompt("\0")+offline(cli.name);
                    announce(clients,msg,cli.id);
                    */
                    assert(cli.fd!=listenfd);
                    close(cli.fd);
                    cli.fd=-1;
                }else{
                    if(cli.cmd and read_buf[0]=='/'){
                        if(!strncmp(read_buf,"/reset\n",7)){
                            str msg;
                            msg=reset();
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }else if(!strncmp(read_buf,"/ping\n",6)){
                            str msg;
                            msg=ping();
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }else if(!strncmp(read_buf,"/report\n",8)){
                            str msg;
                            msg=report();
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }else if(!strncmp(read_buf,"/clients\n",9)){
                            str msg;
                            msg=client(clients);
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                        }else{
                            str msg;
                            str err(read_buf);
                            for(int i=0;i<err.size();++i)
                                if(err[i]=='\n')
                                    err.erase(err.begin()+i,err.end());
                            msg=prompt("\0")+nocmd(err);
                            strcpy(write_buf,msg.c_str());
                            write(cli.fd,write_buf,msg.size());
                            assert(0);
                        }
                    }else if(cli.cmd){
                        assert(read_buf[0]!='\0');
                        cout<<"Not a comand!\n";
                    }else{
                        counter+=readn;
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
