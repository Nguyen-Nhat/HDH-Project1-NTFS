#ifndef _UTILITY_H
#define _UTILITY_H
#include <windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
bool ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to op    en
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return false;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        return false;
    }
    else
    {
        return true;
    }
}
unsigned long long little_endian_to_integer(BYTE* sector, int offset, int size) {
    unsigned long long result = 0, temp;

    for (int i = size - 1; i >= 0; i--) {
        temp = int(sector[offset + i]);
        result = (result << 8) + temp;
    }

    return result;
}
string toString(BYTE* sector, int offset, int size) {
    char* temp = new char[size + 1]; 
    memcpy(temp, sector + offset, size + 1); 
    string result = temp; 
    return result; 
}
#endif // !_UTILITY_H