#include "fileIO.h"
#include "snappy.h"

#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

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
        // If num of files is less than 1000, clear reminding entries.
        if (i < filenames.size()) {
            filesystem::directory_entry file(filenames[i]);

            this->payload->fileEntries[i].contentOffset = contentCounter;
            this->payload->fileEntries[i].size = file.file_size();
            // this->payload->fileEntries[i].checksum = 0;

            uint8_t *contentStart = this->payload->content + contentCounter;

            ifstream fin(filenames[i]);
            fin.read((char *)contentStart, file.file_size());

            // for (uint8_t *it = contentStart; it < contentStart + file.file_size(); it += 2) {
            //     this->payload->fileEntries[i].checksum ^= *(uint16_t *)it;
            // }

            contentCounter += file.file_size();
            contentCounter += 16 - (contentCounter % 16);
        } else {
            this->payload->fileEntries[i].contentOffset = 0;
            this->payload->fileEntries[i].size = 0;
            // this->payload->fileEntries[i].checksum = 0;
        }
    }

    int uncompressedSize = 8 + (32 + 16) * 1000 + contentCounter;
    size_t compressedSize;
    char *rawComp = (char *) malloc(snappy::MaxCompressedLength(uncompressedSize));

    snappy::RawCompress((char *)this->payload, uncompressedSize, rawComp, &compressedSize);

    memcpy(this->payload, rawComp, compressedSize);
    free(rawComp);

    cout << "[FileIO]\t" << "Compressed rate: " << (double)compressedSize / uncompressedSize * 100 << "%" << endl;

    return compressedSize;
}

string fileIO::getFile(int idx) {
    char *tmp = (char *)malloc(this->payload->fileEntries[idx].size);
    memcpy(tmp, (this->payload->content + this->payload->fileEntries[idx].contentOffset), this->payload->fileEntries[idx].size);
    string ret(tmp);
    return ret;
}

string fileIO::genFilename(int num) {
    string ret = to_string(num);
    while (ret.size() < 6)
        ret = "0" + ret;
    return ret;
}

int fileIO::writeFiles(string filePath) {
    char *rawUncomp = (char *) malloc(this->payloadSize);

    bool success = snappy::RawUncompress((char *)this->payload, this->payloadSize, rawUncomp);

    if (!success) {
        cout << "[FileIO]\t Cannot uncompress." << endl;
        return 0;
    }

    memcpy(this->payload, rawUncomp, this->payloadSize);
    free(rawUncomp);

    int fileWritten = 0;
    for (int i = 0; i < 1000; ++i) {
        // If num of files is less than 1000, break the loop.
        if (this->payload->fileEntries[i].size != 0) {
            // uint16_t checksum = 0;

            uint8_t *contentStart = this->payload->content + this->payload->fileEntries[i].contentOffset;

            // for (uint8_t *it = contentStart; it < contentStart + this->payload->fileEntries[i].size; it += 2) {
            //     checksum ^= *(uint16_t *)it;
            // }

            // if (checksum != this->payload->fileEntries[i].checksum) {
            //     cout << "File num " << i << " is broken." << endl;
            //     continue;
            // }

            ofstream fOut(filePath + "/" + genFilename(i), ios::out | ios::binary);
            fOut.write((char *)contentStart, this->payload->fileEntries[i].size);
            fOut.close();
            fileWritten++;
        } else
            break;
    }
    return fileWritten;
}

// int main() {
//     uint8_t *mem = (uint8_t *)malloc(PAYLOAD_SIZE);

//     fileIO tmp(mem, PAYLOAD_SIZE);
//     tmp.readFiles("/root/NYCU-NP-2022-Fall/lab6");
//     tmp.writeFiles("/root/NYCU-NP-2022-Fall/test");

//     return 0;
// }
