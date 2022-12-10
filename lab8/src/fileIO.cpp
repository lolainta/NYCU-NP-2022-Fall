#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

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
                filesystem::directory_entry file(this->filenames[i]);

                this->payload->fileEntries[i].contentOffset = contentCounter;
                this->payload->fileEntries[i].size = file.file_size();
                this->payload->fileEntries[i].checksum = 0;

                contentCounter += file.file_size();
            } else {
                this->payload->fileEntries[i].contentOffset = 0;
                this->payload->fileEntries[i].size = 0;
                this->payload->fileEntries[i].checksum = 0;
            }
        }

        for (auto &it : this->filenames) {
            cout << it.filename() << endl;
            // payload->fileEntries
        }
        return 0;
    }
};

int main() {
    // for (auto &it : fsDirIter) {
    //     cout << it.path().filename() << endl;
    //     ifstream in(it.path(), ios::in | ios::binary);
    //     void *data = calloc(1024, 1);
    //     in.read((char *)data, 1024);
    //     cout << (char *)data << endl << endl;
    // }

    uint8_t *mem = (uint8_t *)malloc(PAYLOAD_SIZE);

    fileIO tmp(mem, PAYLOAD_SIZE);
    tmp.readFiles("/root/NYCU-NP-2022-Fall/lab6");

    return 0;
}