#ifndef FILEIO_H
#define FILEIO_H

#include "payload.h"

#include <vector>
#include <string>
#include <filesystem>

using namespace std;

// string filePath = "/root/NYCU-NP-2022-Fall/lab6";
// int totalFiles = 6;
// int payloadSize = 33554432;

class fileIO {
    dataPayload *payload;
    vector<filesystem::path> filenames;
    unsigned int payloadSize;

  public:
    fileIO(uint8_t *, unsigned int);
    int readFiles(string);
    string getFile(int);
};

/*
int main() {
    uint8_t *mem = (uint8_t *)malloc(PAYLOAD_SIZE);

    fileIO tmp(mem, PAYLOAD_SIZE);
    tmp.readFiles("/root/NYCU-NP-2022-Fall/lab6");
    cout << tmp.getFile(1) << endl;

    return 0;
}
*/
#endif
