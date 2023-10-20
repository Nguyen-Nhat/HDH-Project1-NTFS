#include "Utility.h"
#include "NTFS.h"
int main(){
    BPB * bpb = new BPB();
    init_BPB(bpb, L"\\\\.\\C:");
    print_BPB(bpb);
}