#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <string.h>

#define PAYLOAD_SIZE 33554432

using namespace std;

string filePath = "/root/NYCU-NP-2022-Fall/lab6";
// int totalFiles = 6;
// int payloadSize = 33554432;

struct dataPayload {
    uint8_t notUsed[8];
    struct fileEntry {
        uint32_t contentOffset;
        uint16_t size;
        uint16_t checksum;
    } __attribute__((packed)) fileEntries[1000];
    uint8_t content[PAYLOAD_SIZE];
} __attribute__((packed)) payload;

class fileIO {
    dataPayload *payload;
    vector<filesystem::path> filenames;
    unsigned int payloadSize;

   public:
    fileIO(uint8_t *payload, unsigned int payloadSize) {
        this->payload = (dataPayload *)payload;
        this->payloadSize = payloadSize;
    }
    int readFiles(string filePath) {
        filesystem::path fsPath(filePath);
        filesystem::directory_entry fsDir(fsPath);
        filesystem::directory_iterator fsDirIter(fsDir);

        for (auto &it : fsDirIter)
            this->filenames.emplace_back(it.path());

        sort(filenames.begin(), filenames.end());

        int contentCounter = 0;

        for (int i = 0; i < 1000; ++i) {
            // If num of files is less than 1000, set entries to 0;
            if (i < filenames.size()) {
                cout << this->filenames[i] << endl;
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

        return 0;
    }
    string getFile(int idx) {
        char *tmp = (char *)malloc(this->payload->fileEntries[idx].size);
        memcpy(tmp, (this->payload->content + this->payload->fileEntries[idx].contentOffset), this->payload->fileEntries[idx].size);
        string ret(tmp);
        return ret;
    }
};

int main() {
    uint8_t *mem = (uint8_t *)malloc(PAYLOAD_SIZE);

    fileIO tmp(mem, PAYLOAD_SIZE);
    tmp.readFiles("/root/NYCU-NP-2022-Fall/lab6");
    cout << tmp.getFile(1) << endl;

    return 0;
}