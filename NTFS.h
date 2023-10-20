#ifndef _NTFS_H_
#define _NTFS_H_
#include "Utility.h"

struct BPB {
    unsigned int BytePerSector;
    unsigned int SectorPerCluster;
    unsigned int ReservedSectors;
    unsigned int SectorsPerTrack;
    unsigned int NumOfHead;
    unsigned long long TotalSectors;
    unsigned long long MFTStartCluster;
    unsigned long long MFTMirrStartCluster;
    unsigned int BytePerMFTRecord; 
    unsigned long long VolumeSerialNumber;
};
void print_BPB(BPB* bpb) {
    printf("----------BIOS PARAMETER BLOCK----------\n");
    printf("BytePerSector = %u\n", bpb->BytePerSector);
    printf("SectorPerCluster = %u\n", bpb->SectorPerCluster);
    printf("ReservedSectors = %u\n", bpb->ReservedSectors);
    printf("SectorsPerTrack = %u\n", bpb->SectorsPerTrack);
    printf("NumOfHead = %u\n", bpb->NumOfHead);
    printf("TotalSectors = %llu\n", bpb->TotalSectors);
    printf("MFTStartCluster = %llu\n", bpb->MFTStartCluster);
    printf("MFTMirrStartCluster = %llu\n", bpb->MFTMirrStartCluster);
    printf("BytePerMFTRecord = %u\n", bpb->BytePerMFTRecord);
    printf("VolumeSerialNumber = %llu\n", bpb->VolumeSerialNumber);
}
void init_BPB(BPB* bpb, LPCWSTR disk) {
    BYTE sector[512];
    ReadSector(disk, 0, sector);
    bpb->BytePerSector = (unsigned int)little_endian_to_integer(sector, 11, 2);
    bpb->SectorPerCluster = (unsigned int)little_endian_to_integer(sector, 13, 1);
    bpb->ReservedSectors = (unsigned int)little_endian_to_integer(sector, 14, 2);
    bpb->SectorsPerTrack = (unsigned int)little_endian_to_integer(sector, 24, 2);
    bpb->NumOfHead = (unsigned int)little_endian_to_integer(sector, 26, 2);
    bpb->TotalSectors = (unsigned long long)little_endian_to_integer(sector, 40, 8);
    bpb->MFTStartCluster = (unsigned long long)little_endian_to_integer(sector, 48, 8);
    bpb->MFTMirrStartCluster = (unsigned long long)little_endian_to_integer(sector, 56, 8);
    bpb->VolumeSerialNumber = (unsigned long long)little_endian_to_integer(sector, 72, 8);
    bpb->BytePerMFTRecord = pow(2, abs(char(sector[64])));
}
#endif