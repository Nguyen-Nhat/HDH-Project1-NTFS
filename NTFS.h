#ifndef _NTFS_H_
#define _NTFS_H_
#include "Utility.h"

struct PBS {
    BYTE sector[512];
    unsigned char JumpInstruction[3];
    string Name;
    unsigned int BytePerSector;
    unsigned int SectorPerCluster;
    unsigned char MediaDescriptor;
    unsigned int ReservedSectors;
    unsigned int SectorsPerTrack;
    unsigned int NumOfHead;
    unsigned int HiddenSectors;
    unsigned long long TotalSectors;
    unsigned long long MFTStartCluster;
    unsigned long long MFTMirrStartCluster;
    unsigned int BytePerMFTEntry;
    unsigned char ClustersPerIndexBuffer;
    unsigned long long VolumeSerialNumber;
    LPCWSTR Disk;
    bool CheckNTFS();
    void print_PBS();
    void read_PBS(LPCWSTR disk);
};

enum NTFSAttribute {
    READ_ONLY = 1 << 0,
    HIDDEN = 1 << 1,
    SYSTEM = 1 << 2,
    VOLLABLE = 1 << 3,
    DIRECTORY = 1 << 4,
    ARCHIVE = 1 << 5,
    DEVICE = 1 << 6,
    NORMAL = 1 << 7,
    TEMPORARY = 1 << 8,
    SPARSE_FILE = 1 << 9,
    REPARSE_POINT = 1 << 10,
    COMPRESSED = 1 << 11,
    OFFLINE = 1 << 12,
    NOT_INDEXED = 1 << 13,
    ENCRYPTED = 1 << 14
};
struct StandardInfoAttr {
    int Flags; 
    char* CreateTime; // 0 - 7
    char* LastModifiedTime; // 8 - 15
    char* LastAccessTime;  // 24 - 31 
    StandardInfoAttr();
    void print();
};
struct FileNameAttr {
    unsigned int Size; // filenamestart + 16 -> 4
    unsigned int Offset; // filenamestart + 20 -> 2
    unsigned int ParentMFTEntry; // filenamestart + offset -> 8
    wchar_t* LongName; // offset + 66 -> Size - 66 
    FileNameAttr();
    void print();
};
struct DataAttr {
    bool Resident; // true -> resident , false -> non-resident 
    unsigned long long Size;
    int typeText; // 1 -> utf16, 0 -> ascii
    wchar_t* DataUTF16; 
    char* DataASCII; 
    vector<unsigned long long> Lengths; 
    vector<unsigned long long> Offsets; 
    DataAttr();
    void print();
};
struct MFTEntry {
    BYTE* RawData;
    unsigned int FileId; // 0x2C->4
    unsigned char Flag;  // 0x16->1 
    unsigned int StandardInfoStart; // 0x14->2
    unsigned long long StandardInfoSize; // infoStart + 4 -> 4
    unsigned int FileNameStart; // infostart + infosize;
    unsigned long long FileNameSize; // filenameStart + 4 -> 4
    unsigned int DataStart; // filenamestart + filenamesize
    unsigned int DataId; 
    StandardInfoAttr StandardInfo;
    FileNameAttr FileName;
    DataAttr Data;
    vector<MFTEntry*> childs; 
    MFTEntry(BYTE* rawData);
    bool IsDirectory();
    bool IsLeaf();
    bool IsActiveRecord();
    void AnalyzeStandardInfo();
    void AnalyzeFileName();
    void AnalyzeData();
    void printRecord();
    void printTree(int depth = 1);
    void printListChild();
};
struct MFTFile {
    unsigned int InfoOffset; 
    unsigned int InfoLength;
    unsigned int FileNameOffset;
    unsigned int FileNameLength;
    unsigned int DataOffset;
    unsigned int DataLength;
    unsigned long long NumSector; 
    MFTFile();
    MFTFile(PBS* pbs);
};
struct NTFS {
    PBS* pbs; 
    MFTFile mftFile; 
    vector<MFTEntry*> MFTEntrys;
    int root; 
    int current;
    unordered_map<int, MFTEntry*> dictionary;
    void Init(LPCWSTR disk);
    void printBootSector();
    void printCurrentTree();
    void printTXTFile();
    void changeDirectory(int order);
    void backParentDirectory();
    void readFile();
    void showListChild();
    wstring showPath();
    void Finish();
};
#endif