#ifndef _UTILITY_H
#define _UTILITY_H
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <unordered_map>
#include <vector>
#include <io.h>
#include <wchar.h>
#include <bitset>
using namespace std;
// read one sector 
void ReadSector(LPCWSTR  drive, unsigned int readPoint, BYTE* sector);
// read multiple sectors 
void ReadMultipleSector(LPCWSTR  drive, unsigned int readPoint, unsigned int numSec, BYTE* sector);
// convert little endian bytes to integer
unsigned long long little_endian_to_integer(BYTE* sector, int offset, int size);
// convert little endian bytes to datetime
char* little_endian_to_datetime(BYTE* sector, int offset);
// convert little endian bytes to string
string toString(BYTE* DATA, int offset, int number);
wstring getAppOpen(wstring ext);
#endif // !_UTILITY_H