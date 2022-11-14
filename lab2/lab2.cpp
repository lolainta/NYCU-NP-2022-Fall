#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

struct FILE_E{
    int str;
    int size;
    int cont;
    uint64_t checksum=0;
    string filename;
};

FILE_E parseFE(ifstream&file){
    FILE_E ret;
    file.read((char*)&ret.str,sizeof(int));
    file.read((char*)&ret.size,sizeof(int));
    file.read((char*)&ret.cont,sizeof(int));
    file.read((char*)&ret.checksum,8);
    ret.size=__builtin_bswap32((uint32_t)ret.size);
    ret.checksum=__builtin_bswap64((uint64_t)ret.checksum);
    cout<<ret.str<<' '<<ret.size<<' '<<ret.cont<<' '<<ret.checksum<<endl;
    return ret;
}

bool check(long long ans,char*st,int sz){
    vector<uint8_t> c(8);
    for(int i=0;i<sz;++i)
        c[i%8]^=st[i];
    for(int i=0;i<8;++i)
        if((ans>>i*8&0xff)-c[i])
            return false;
    return true;
}

int main(int argc,char*argv[]){
    string pak(argv[1]);
    string folder=string(argv[2])+"/";
    cout<<hex;

    ifstream ifile(pak,ios::in|ios::binary);

    char h1[5];
    ifile.read(h1,sizeof(char)*4);
    cout<<h1<<' ';
    int h2;
    ifile.read((char*)&h2,sizeof(h2));
    cout<<h2<<' ';
    int h3;
    ifile.read((char*)&h3,sizeof(h3));
    cout<<h3<<' ';
    int h4;
    ifile.read((char*)&h4,sizeof(h4));
    cout<<h4<<endl;

    vector<FILE_E> fe(h4);

    for(int i=0;i<h4;++i)
        fe[i]=parseFE(ifile);
    
    char*filesection=(char*)calloc((h3-h2),1);
    ifile.read(filesection,h3-h2);
    for(int i=0;i<h4;++i)
        fe[i].filename=string(filesection+fe[i].str);

    int content_size=(int)ifstream(pak,ios::ate|ios::binary).tellg()-h3;
    char*content=(char*)calloc(content_size,1);
    ifile.read(content,content_size);
    ifile.close();
    for(int i=0;i<h4;++i){
        if(check(fe[i].checksum,content+fe[i].cont,fe[i].size)){
            ofstream(folder+fe[i].filename,ios::out|ios::binary).write(content+fe[i].cont,fe[i].size);
            cout<<"(O)"<<fe[i].filename<<endl;
        }else{
            cout<<"(X)"<<fe[i].filename<<endl;
        }
    }

    return 0;
}
