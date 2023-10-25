#include "Utility.h"

void ReadSector(LPCWSTR  drive, unsigned int readPoint, BYTE* sector)
{
    DWORD bytesRead(0);

    HANDLE device = NULL;
    device = CreateFile(drive,    // Floppy drive to open
        GENERIC_READ,              // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,           // Share Mode
        NULL,                      // Security Descriptor
        OPEN_EXISTING,             // How to create
        0,                         // File attributes
        NULL);                     // Handle to template

    if (device != NULL)
    {
        LARGE_INTEGER li;
        li.QuadPart = readPoint * 512;
        SetFilePointerEx(device, li, 0, FILE_BEGIN);
        if (!ReadFile(device, sector, 512, &bytesRead, NULL))
        {
            printf("Error in reading floppy disk\n");
            exit(1);
        }
        CloseHandle(device);
    }
}
void ReadMultipleSector(LPCWSTR  drive, unsigned int readPoint, unsigned int numSec, BYTE* sector)
{
    DWORD bytesRead(0);

    HANDLE device = NULL;
    device = CreateFile(drive,    // Floppy drive to open
        GENERIC_READ,              // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,           // Share Mode
        NULL,                      // Security Descriptor
        OPEN_EXISTING,             // How to create
        0,                         // File attributes
        NULL);                     // Handle to template

    if (device != NULL)
    {
        LARGE_INTEGER li;
        li.QuadPart = readPoint * 512;
        SetFilePointerEx(device, li, 0, FILE_BEGIN);
        if (!ReadFile(device, sector, 512 * numSec, &bytesRead, NULL))
        {
            printf("Error in reading floppy disk\n");
            exit(1);
        }
        CloseHandle(device);
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
char* little_endian_to_datetime(BYTE* sector, int offset) {
    time_t result = (little_endian_to_integer(sector, offset, 8) - 116444736000000000) / 10000000;
    char* temp = ctime(&result);
    return temp;
}
string toString(BYTE* DATA, int offset, int number)
{
    char* tmp = new char[number + 1];
    memcpy(tmp, DATA + offset, number);
    string s = "";
    for (int i = 0; i < number; i++)
        if (tmp[i] != 0x00)
            s += tmp[i];

    return s;
}
wstring getAppOpen(wstring ext)
{
    wstring opener = L"";

    if (ext == L"txt" || ext == L"TXT")
        return L"";
    if (ext == L"doc" || ext == L"DOC" || ext == L"docx" || ext == L"DOCX")
        opener = L"Microsoft Word";
    if (ext == L"xls" || ext == L"XLS" || ext == L"xlsx" || ext == L"XLSX")
        opener = L"Microsoft Excel";
    if (ext == L"ppt" || ext == L"PPT" || ext == L"pptx" || ext == L"PPTX")
        opener = L"Microsoft Powerpoint";
    if (ext == L"pdf" || ext == L"PDF")
        opener = L"Adobe Reader XI";
    if (ext == L"rtf" || ext == L"RTF")
        opener = L"WordPad";
    if (ext == L"mp3" || ext == L"MP3" || ext == L"mp4" || ext == L"MP4" || ext == L"wmv" || ext == L"WMV" || ext == L"mpeg" || ext == L"MPQG"
        || ext == L"wav" || ext == L"WAV" || ext == L"ogg" || ext == L"OGG" || ext == L"ts" || ext == L"TS")
        opener = L"Windows Media Player";
    if (ext == L"jpg" || ext == L"JPG" || ext == L"jpeg" || ext == L"JPEG" || ext == L"png" || ext == L"PNG" || ext == L"svg" || ext == L"SVG")
        opener = L"Photos";;
    if (ext == L"sql" || ext == L"SQL")
        opener = L"Microsoft SQL Server";
    if (ext == L"rar" || ext == L"RAR" || ext == L"rar4" || ext == L"RAR4" || ext == L"zip" || ext == L"ZIP")
        opener = L"WinRAR";
    return L"Using: " + opener + L" to open " + ext + L" file.";
}
