#ifndef FILEIO_H
#define FILEIO_H

#include <filesystem>
#include <string>
#include <vector>

#include "payload.h"

using namespace std;

class fileIO {
    dataPayload *payload;
    unsigned int payloadSize;

   public:
    fileIO(uint8_t *, unsigned int);
    int readFiles(string);
    string getFile(int);
};

#endif
