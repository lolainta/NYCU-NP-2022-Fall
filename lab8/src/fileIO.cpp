#include "fileIO.h"

#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string filePath = "/root/NYCU-NP-2022-Fall/lab6";
// int totalFiles = 6;
// int payloadSize = 33554432;

fileIO::fileIO(uint8_t *payload, unsigned int payloadSize) {
    this->payload = (dataPayload *)payload;
    this->payloadSize = payloadSize;
}

int fileIO::readFiles(string filePath) {
    filesystem::path fsPath(filePath);
    filesystem::directory_entry fsDir(fsPath);
    filesystem::directory_iterator fsDirIter(fsDir);

    vector<filesystem::path> filenames;

    for (auto &it : fsDirIter)
        filenames.emplace_back(it.path());

    sort(filenames.begin(), filenames.end());

    int contentCounter = 0;

    for (int i = 0; i < 1000; ++i) {
        // If num of files is less than 1000, set entries to 0;
        if (i < filenames.size()) {
            cout << filenames[i] << endl;
            filesystem::directory_entry file(filenames[i]);

            this->payload->fileEntries[i].contentOffset = contentCounter;
            this->payload->fileEntries[i].size = file.file_size();
            this->payload->fileEntries[i].checksum = 0;

            ifstream fin(filenames[i]);
            fin.read((char *)(this->payload->content + contentCounter), file.file_size());

            for (uint8_t *it = this->payload->content + contentCounter; it < this->payload->content + contentCounter + file.file_size(); it += 2) {
                this->payload->fileEntries[i].checksum ^= *(uint16_t *)it;
            }

            contentCounter += file.file_size();
        } else {
            this->payload->fileEntries[i].contentOffset = 0;
            this->payload->fileEntries[i].size = 0;
            this->payload->fileEntries[i].checksum = 0;
        }
    }

    return 0;
}

string fileIO::getFile(int idx) {
    char *tmp = (char *)malloc(this->payload->fileEntries[idx].size);
    memcpy(tmp, (this->payload->content + this->payload->fileEntries[idx].contentOffset), this->payload->fileEntries[idx].size);
    string ret(tmp);
    return ret;
}

int main() {
    uint8_t *mem = (uint8_t *)malloc(PAYLOAD_SIZE);

    fileIO tmp(mem, PAYLOAD_SIZE);
    tmp.readFiles("/root/NYCU-NP-2022-Fall/lab6");
    cout << tmp.getFile(1) << endl;

    return 0;
}
