#include "fileIO.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <vector>

using namespace std;

fileIO::fileIO(uint8_t*payload,unsigned int _payloadSize):payloadSize(_payloadSize){
    this->payload=(dataPayload*)payload;
}

int fileIO::readFiles(string filePath){
    filesystem::path fsPath(filePath);
    filesystem::directory_entry fsDir(fsPath);
    filesystem::directory_iterator fsDirIter(fsDir);

    vector<filesystem::path> fileNames;
    for (auto&it:fsDirIter)
        fileNames.emplace_back(it.path());
    sort(fileNames.begin(),fileNames.end());

    int contentCounter=0;
    for(size_t i=0;i<fileNames.size();++i){
        auto&fname=fileNames[i];
        filesystem::directory_entry file(fname);
        this->payload->fileEntries[i].contentOffset=contentCounter;
        this->payload->fileEntries[i].size=file.file_size();
        ifstream fin(fname);
        fin.read((char*)payload->content+contentCounter,file.file_size());
        fin.close();
        contentCounter+=file.file_size();
        contentCounter += (contentCounter % 16 == 0) ? 0 : 16 - (contentCounter % 16);
    }
    return 4+(4+2)*1000+contentCounter;
}

string fileIO::getFile(int idx){
    char*tmp=(char*)malloc(this->payload->fileEntries[idx].size);
    memcpy(tmp,(this->payload->content+this->payload->fileEntries[idx].contentOffset),this->payload->fileEntries[idx].size);
    string ret(tmp);
    return ret;
}

string fileIO::genFilename(int num){
    string ret=to_string(num);
    while(ret.size()<6)
        ret="0"+ret;
    return ret;
}

int fileIO::writeFiles(string filePath){
    int fileWritten=0;
    for(size_t i=0;i<1000;++i){
//        std::cout<<i<<' '<<payload->fileEntries[i].size<<' '<<payload->fileEntries[i].contentOffset<<std::endl;
        if(this->payload->fileEntries[i].size!=0){
            uint8_t*contentStart=this->payload->content+this->payload->fileEntries[i].contentOffset;
            ofstream fout(filePath+"/"+genFilename(i));
            fout.write((char*)contentStart,this->payload->fileEntries[i].size);
            fout.close();
            fileWritten++;
        }else
            break;
    }
    return fileWritten;
}
