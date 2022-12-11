#include "fileIO.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string.h>


fileIO::fileIO(uint8_t *payload, unsigned int payloadSize) {
    this->payload = (dataPayload *)payload;
    this->payloadSize = payloadSize;
}

int fileIO::readFiles(string filePath) {
    filesystem::path fsPath(filePath);
    filesystem::directory_entry fsDir(fsPath);
    filesystem::directory_iterator fsDirIter(fsDir);

    for (auto &it : fsDirIter)
        this->filenames.emplace_back(it.path());

    sort(filenames.begin(), filenames.end());

    int contentCounter = 0;

    for (int i = 0; i < 1000; ++i) {
            // If num of files is less than 1000, set entries to 0;
        if (i < (int)filenames.size()) {
            // cout << this->filenames[i] << endl;
            filesystem::directory_entry file(this->filenames[i]);

            this->payload->fileEntries[i].contentOffset = contentCounter;
            this->payload->fileEntries[i].size = file.file_size();
            this->payload->fileEntries[i].checksum = 0;

            ifstream fin(this->filenames[i]);
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
        // cout << (char *)this->payload->content << endl;
    }

    return contentCounter;
}

string fileIO::getFile(int idx) {
    char *tmp = (char *)malloc(this->payload->fileEntries[idx].size);
    memcpy(tmp, (this->payload->content + this->payload->fileEntries[idx].contentOffset), this->payload->fileEntries[idx].size);
    string ret(tmp);
    return ret;
}

