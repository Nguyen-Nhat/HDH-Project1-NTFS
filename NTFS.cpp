#include "NTFS.h"

bool PBS::CheckNTFS(){
    if (Name == "NTFS    ")
            return true;
        return false;
}
void PBS::read_PBS(LPCWSTR disk) {
    ReadSector(disk, 0, sector);
    Name = toString(sector, 03, 8);
    JumpInstruction[0] = (char)sector[0];
    JumpInstruction[1] = (char)sector[1];
    JumpInstruction[2] = (char)sector[2];
    BytePerSector = (unsigned int)little_endian_to_integer(sector, 11, 2);
    SectorPerCluster = (unsigned int)little_endian_to_integer(sector, 13, 1);
    MediaDescriptor = (char)little_endian_to_integer(sector, 0x15, 1);
    ReservedSectors = (unsigned int)little_endian_to_integer(sector, 14, 2);
    SectorsPerTrack = (unsigned int)little_endian_to_integer(sector, 24, 2);
    NumOfHead = (unsigned int)little_endian_to_integer(sector, 26, 2);
    HiddenSectors = (unsigned int)little_endian_to_integer(sector, 0x1C, 4);
    TotalSectors = (unsigned long long)little_endian_to_integer(sector, 40, 8);
    MFTStartCluster = (unsigned long long)little_endian_to_integer(sector, 48, 8);
    MFTMirrStartCluster = (unsigned long long)little_endian_to_integer(sector, 56, 8);
    VolumeSerialNumber = (unsigned long long)little_endian_to_integer(sector, 72, 8);
    ClustersPerIndexBuffer = sector[0x44];
    BytePerMFTEntry = char(sector[64]) > 0 ? char(sector[64]) : pow(2, abs(char(sector[64])));
    Disk = disk;
}
void PBS::print_PBS() {
    printf("\tPARTITION BOOT SECTOR TABLE OF NTFS\n\n");
    printf("       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    printf("      -----------------------------------------------\n");
    for (int i = 0; i < 512; i += 16) {
        printf("%03X |", i);
        for (int j = 0; j < 16; j++) {
            printf(" %02X", sector[i + j]);
        }
        printf("\n");
    }
    printf("\n\n");
    printf("\t\tBIOS PARAMETER BLOCK\n\n");
    printf("Jump instruction to boot code: %X %X %X\n", JumpInstruction[0], JumpInstruction[1], JumpInstruction[2]);
    cout << "Name: " << Name << endl;
    printf("Bytes Per Sector : 0x%X (%u)\n", BytePerSector, BytePerSector);
    printf("Sectors Per Cluster : 0x%X (%u)\n", SectorPerCluster, SectorPerCluster);
    printf("Media Descriptor: 0x%X (%u)\n", MediaDescriptor, MediaDescriptor);
    printf("Reserved Sectors : 0x%X (%u)\n", ReservedSectors, ReservedSectors);
    printf("Sectors Per Track : 0x%X (%u)\n", SectorsPerTrack, SectorsPerTrack);
    printf("Number Of Head : 0x%X (%u)\n", NumOfHead, NumOfHead);
    printf("Hidden Sectors : 0x%X (%u)\n", HiddenSectors, HiddenSectors);
    printf("Total Sectors : 0x%016llX (%llu)\n", TotalSectors, TotalSectors);
    printf("MFTStartCluster : 0x%016llX (%llu)\n", MFTStartCluster, MFTStartCluster);
    printf("MFTMirrStartCluster : 0x%016llX (%llu)\n", MFTMirrStartCluster, MFTMirrStartCluster);
    printf("Bytes Per MFT Record : %u\n", BytePerMFTEntry);
    printf("Clusters Per Index Buffer: 0x%X (%u)\n", ClustersPerIndexBuffer, ClustersPerIndexBuffer);
    printf("Volume Serial Number : 0x%016llX (%llu)\n", VolumeSerialNumber, VolumeSerialNumber);
}

StandardInfoAttr::StandardInfoAttr() {
    Flags = 0;
    CreateTime = NULL;
    LastModifiedTime = NULL;
    LastAccessTime = NULL;
}
void StandardInfoAttr::print() {
    cout << "Flag: " << bitset<16>(Flags) << endl;
    cout << "CreateTime: " << CreateTime << endl;
    cout << "LastModifiedTime: " << LastModifiedTime << endl;
}


FileNameAttr::FileNameAttr() {
    Size = 0;
    Offset = 0;
    LongName = NULL;
    ParentMFTEntry = 0;
}
void FileNameAttr::print() {
    printf("ParentMFTEntry: %llu\n", ParentMFTEntry);
    _setmode(_fileno(stdout), _O_U8TEXT);
    wprintf(L"Long File Name: %s\n", LongName);
    _setmode(_fileno(stdout), _O_TEXT);
}

DataAttr::DataAttr() {
    Resident = 0;
    Size = 0;
    DataUTF16 = NULL;
    DataASCII = NULL;
}
void DataAttr::print() {
    cout << "Resident: " << Resident << endl;
    cout << "Size: " << Size << endl;
}

MFTEntry::MFTEntry(BYTE* rawData) {
    RawData = rawData;
    if (toString(rawData, 0, 4) != "FILE") {
        string s = "Skip record";
        throw s;
    }
    FileId = little_endian_to_integer(RawData, 0x2C, 4);
    Flag = little_endian_to_integer(RawData, 0x16, 1);
    if (Flag == 0 || Flag == 2) {
        string s = "Skip record";
        throw s;
    }
    StandardInfoStart = little_endian_to_integer(RawData, 0x14, 2);
    StandardInfoSize = little_endian_to_integer(RawData, StandardInfoStart + 4, 4);
    FileNameStart = StandardInfoStart + StandardInfoSize;
    FileNameSize = little_endian_to_integer(RawData, FileNameStart + 4, 4);
    try {
        AnalyzeStandardInfo();
        AnalyzeFileName();
    }
    catch (string error) {
        string s = "Skip record";
        throw s;
    }
    DataStart = FileNameStart + FileNameSize;
    DataId = little_endian_to_integer(RawData, DataStart, 4);
    if (DataId == 64) {
        DataStart += little_endian_to_integer(RawData, DataStart + 4, 4);
    }
    DataId = little_endian_to_integer(RawData, DataStart, 4);
    if (DataId == 128) {
        AnalyzeData();
    }
    else if (DataId == 144) {
        StandardInfo.Flags |= DIRECTORY;
        Data.Size = 0;
        Data.Resident = true;
    }

}

bool MFTEntry::IsDirectory() {
    return (StandardInfo.Flags & DIRECTORY) > 0;
}
bool MFTEntry::IsLeaf() {
    return childs.empty();
}
bool MFTEntry::IsActiveRecord() {
    if (StandardInfo.Flags & SYSTEM || StandardInfo.Flags & HIDDEN)
        return false;
    return true;
}
void MFTEntry::AnalyzeStandardInfo() {
    int sig = little_endian_to_integer(RawData, StandardInfoStart, 4);
    if (sig != 0x10) {
        string err = "error";
        throw err;
    }
    int offset = little_endian_to_integer(RawData, StandardInfoStart + 20, 2);
    StandardInfo.Flags = little_endian_to_integer(RawData, StandardInfoStart + offset + 32, 4);
    StandardInfo.CreateTime = little_endian_to_datetime(RawData, StandardInfoStart + offset);
    StandardInfo.LastModifiedTime = little_endian_to_datetime(RawData, StandardInfoStart + offset + 8);
    StandardInfo.LastAccessTime = little_endian_to_datetime(RawData, StandardInfoStart + offset + 24);
}
void MFTEntry::AnalyzeFileName() {
    int sig = little_endian_to_integer(RawData, FileNameStart, 4);
    if (sig != 0x30) {
        string err = "error";
        throw err;
    }
    FileName.Size = little_endian_to_integer(RawData, FileNameStart + 16, 4);
    FileName.Offset = little_endian_to_integer(RawData, FileNameStart + 20, 2);
    FileName.ParentMFTEntry = little_endian_to_integer(RawData, FileNameStart + FileName.Offset, 6);
    int temp = (FileName.Size - 66) / 2;
    FileName.LongName = new wchar_t[temp + 1];
    for (int i = 0; i < temp; i++)
        memcpy(&FileName.LongName[i], RawData + FileNameStart + FileName.Offset + 66 + i * 2, 2);
    FileName.LongName[temp] = L'\0';
}
void MFTEntry::AnalyzeData() {
    Data.Resident = !bool(RawData[DataStart + 0x8]);
    if (Data.Resident) {
        unsigned int offset = little_endian_to_integer(RawData, DataStart + 0x14, 2);
        Data.Size = little_endian_to_integer(RawData, DataStart + 0x10, 4);
        bool isLittleEndian = true;
        Data.typeText = 1;
        if (RawData[DataStart + offset] == 0xFE && RawData[DataStart + offset + 1] == 0xFF)
            isLittleEndian = false;
        else if (RawData[DataStart + offset] == 0xFF && RawData[DataStart + offset + 1] == 0xFE)
            isLittleEndian = true;
        else Data.typeText = 0;
        if (Data.typeText == 1) {
            int temp = Data.Size / 2;
            Data.DataUTF16 = new wchar_t[temp];
            Data.DataUTF16[temp - 1] = L'\0';
            for (int i = 1; i < temp; i++) {
                if (isLittleEndian)
                    Data.DataUTF16[i - 1] = (RawData[DataStart + offset + i * 2 + 1] << 8) | RawData[DataStart + offset + i * 2];
                else
                    Data.DataUTF16[i - 1] = (RawData[DataStart + offset + i * 2] << 8) | RawData[DataStart + offset + i * 2 + 1];
            }
        }
        else {
            Data.DataASCII = new char[Data.Size + 1];
            Data.DataASCII[Data.Size] = '\0'; 
            for (int i = 0; i < Data.Size; i++) {
                Data.DataASCII[i] = RawData[DataStart + offset + i];
            }
        }
        
    }
    else {
        Data.Size = little_endian_to_integer(RawData, DataStart + 0x30, 8);
        int temp = 0;
        unsigned int s = DataStart + 0x40;
        while (RawData[s] != 0) {
            int offset = (RawData[s] & 0xF0) >> 4;
            int size = RawData[s] & 0x0F;
            Data.Lengths.push_back(little_endian_to_integer(RawData, s + 1, size));
            Data.Offsets.push_back(little_endian_to_integer(RawData, s + 1 + size, offset));
            s += 1 + offset + size;
        }
    }
}
void MFTEntry::printRecord() {
    printf("FileId: %u\n", FileId);
    printf("StandardInfoStart: %u\n", StandardInfoStart);
    printf("StandardInfoSize: %llu\n", StandardInfoSize);
    printf("FileNameStart: %u\n", FileNameStart);
    printf("FileNameSize: %llu\n", FileNameSize);
    printf("DataStart: %u\n", DataStart);
    printf("DataId: %u\n", DataId);
    StandardInfo.print();
    FileName.print();
    Data.print();
}
void MFTEntry::printTree(int depth) {
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i]->IsActiveRecord()) {
            for (int i = 0; i < depth; i++)
                printf("  ");
            if (childs[i]->IsDirectory())
                printf("* ");
            else
                printf("+ ");
            _setmode(_fileno(stdout), _O_U8TEXT);
            wprintf(L"%s\n", childs[i]->FileName.LongName);
            _setmode(_fileno(stdout), _O_TEXT);
            childs[i]->printTree(depth + 1);
        }
    }
}
void MFTEntry::printListChild() {
    int temp = 1;
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i]->IsActiveRecord()) {
            printf("%d", temp);
            printf("  ");
            if (childs[i]->IsDirectory())
                printf("* ");
            else
                printf("+ ");
            _setmode(_fileno(stdout), _O_U8TEXT);
            wprintf(L"%s\n", childs[i]->FileName.LongName);
            _setmode(_fileno(stdout), _O_TEXT);
            temp++; 
        }
    }
}
MFTFile::MFTFile() {

}
MFTFile::MFTFile(PBS* pbs) {
    BYTE* mft = new BYTE[pbs->BytePerMFTEntry];
    ReadMultipleSector(pbs->Disk, pbs->SectorPerCluster * pbs->MFTStartCluster,
        pbs->BytePerMFTEntry / pbs->BytePerSector, mft);
    InfoOffset = little_endian_to_integer(mft, 0x14, 2);
    InfoLength = little_endian_to_integer(mft, 0x3c, 4);
    FileNameOffset = InfoOffset + InfoLength;
    FileNameLength = little_endian_to_integer(mft, 0x9c, 4);
    DataOffset = FileNameOffset + FileNameLength;
    DataLength = little_endian_to_integer(mft, 0x104, 4);
    NumSector = little_endian_to_integer(mft, 0x118, 2) * 8 + 8;
    delete[] mft;
}



void NTFS::Init(LPCWSTR disk) {
    pbs = new PBS();
    pbs->read_PBS(disk);
    if (pbs->CheckNTFS()) {
        cout << "Success\n";
    }
    else {
        cout << "Incorrect NTFS type\n";
        exit(1);
    }
    mftFile = MFTFile(pbs);
    for (int i = 0; i < mftFile.NumSector; i += 2) {
        BYTE* rawData = new BYTE[1024];
        ReadMultipleSector(pbs->Disk, pbs->SectorPerCluster * pbs->MFTStartCluster + i, 2, rawData);
        try
        {
            MFTEntrys.push_back(new MFTEntry(rawData));
        }
        catch (string s) {}
    }

    root = 5;
    current = 5;
    for (int i = 0; i < MFTEntrys.size(); i++) {
        dictionary[MFTEntrys[i]->FileId] = MFTEntrys[i];
    }
    for (int i = 0; i < MFTEntrys.size(); i++) {
        int key = MFTEntrys[i]->FileId;
        if (key != 5) {
            dictionary[MFTEntrys[i]->FileName.ParentMFTEntry]->childs.push_back(MFTEntrys[i]);
        }
    }
}
void NTFS::printBootSector() {
    pbs->print_PBS();
}
void NTFS::printCurrentTree() {
    dictionary[current]->printTree();
}
void NTFS::printTXTFile() {
    MFTEntry* node = dictionary[current];
    if (node->Data.Resident) {
        if (node->Data.typeText == 1) {
            _setmode(_fileno(stdout), _O_U8TEXT);
            wprintf(L"%s\n", node->Data.DataUTF16);
            _setmode(_fileno(stdout), _O_TEXT);
        }
        else {
            printf("%s\n", node->Data.DataASCII);
        }
    }
    else {
        int numChar = node->Data.Size / 2;
        wchar_t* dataUTF16 = new wchar_t[numChar + 1];
        dataUTF16[numChar] = L'\0';
        char* dataASCII = NULL;
        bool isLittleEndian = true;
        BYTE* clusterData = new BYTE[512 * pbs->SectorPerCluster];
        int index = 0;
        node->Data.typeText = 1; 
        for (int i = 0; i < node->Data.Lengths.size(); i++) {
            int numCluster = node->Data.Lengths[i];
            int clusterOffset = node->Data.Offsets[i];
            for (int j = 0; j < numCluster; j++) {
                ReadMultipleSector(pbs->Disk, (clusterOffset + j) * pbs->SectorPerCluster, 8, clusterData);
                int k = 0;
                if (i == 0 && j == 0) {
                    k = 1;
                    if (clusterData[0] == 0xFE && clusterData[1] == 0xFF)
                        isLittleEndian = false;
                    else if (clusterData[0] == 0xFF && clusterData[1] == 0xFE)
                        isLittleEndian = true;
                    else {
                        node->Data.typeText = 0;
                        dataASCII = new char[node->Data.Size + 1];
                        dataASCII[node->Data.Size] = '\0';
                    }

                }
                if (node->Data.typeText == 1) {
                    for (; k < 512 * pbs->SectorPerCluster / 2 && index < numChar; k++, index++) {
                        if (isLittleEndian) {
                            dataUTF16[index] = (clusterData[k * 2 + 1] << 8 | clusterData[k * 2]);
                        }
                        else {
                            dataUTF16[index] = (clusterData[k * 2] << 8 | clusterData[k * 2 + 1]);
                        }
                    }
                }
                else {
                    for (int k = 0; k < 512 * pbs->SectorPerCluster && index < node->Data.Size; k++, index++) {
                        dataASCII[index] = clusterData[k];
                    }
                }
            }
        }
        if (node->Data.typeText == 1) {
            _setmode(_fileno(stdout), _O_U8TEXT);
            wprintf(L"%s\n", dataUTF16);
            _setmode(_fileno(stdout), _O_TEXT);
        }
        else {
            printf("%s\n", dataASCII);
            delete[] dataASCII; 
        }
        delete[] dataUTF16;
        delete[] clusterData;
    }
}
void NTFS::changeDirectory(int order) {
    MFTEntry* node = dictionary[current];
    bool find = false;
    for (int i = 0; i < node->childs.size(); i++) {
        if (node->childs[i]->IsActiveRecord()) order--;
        if (order == 0) {
            find = true; 
            current = node->childs[i]->FileId;
        }
    }
    if (!find) {
        cout << "Wrong index\n";
    }
}
void NTFS::backParentDirectory() {
    current = dictionary[current]->FileName.ParentMFTEntry;
}
void NTFS::showListChild() {
    dictionary[current]->printListChild();
}
wstring NTFS::showPath() {
    wstring disk = wstring(pbs->Disk);
    disk = disk.substr(4, disk.length() - 5);
    MFTEntry* node = dictionary[current];
    wstring result(node->FileName.LongName);
    if (node->FileId == root) {
        result = disk; 
    }
    unsigned int key = node->FileName.ParentMFTEntry; 
    while (node->FileId != root) {
        node = dictionary[key];

        wstring buffer(node->FileName.LongName);
        if (node->FileId == root) {
            buffer = disk;
        }
        result = buffer + L'\\' + result;

        key = node->FileName.ParentMFTEntry;
    }
    return result;
}
void NTFS::readFile() {
    if (dictionary[current]->IsDirectory()) {
        cout << "This is directory\n";
    }
    else {
        wstring filename(dictionary[current]->FileName.LongName);
        wstring ext = filename.substr(filename.find('.') + 1);
        wstring msg = getAppOpen(ext);
        if (msg.length() == 0) {
            printTXTFile();
        }
        else {
            wcout << msg;
        }
        cout << endl;
    }
}
void NTFS::Finish() {
    for (int i = 0; i < MFTEntrys.size(); i++) {
        delete MFTEntrys[i];
    }
}