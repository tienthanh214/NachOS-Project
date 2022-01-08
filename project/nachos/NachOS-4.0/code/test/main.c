/* main.c
    chuong trinh quan ly sinh vien voi nuoc
    thong ke su dung may nong lanh
*/

#include "syscall.h"
#include "fileutils.h"

int main() {
    int nTest;
    int isSuccess;
    int readStatus;
    int a[5], n, i;
    // // semaphore quan ly khi co yeu cau uong nuoc
    // if (!(isSuccess = CreateSemaphore("open_water", 0)))
    //     return 1;   // loi khong the tao semaphore
    // // semaphore quan ly 2 voi nuoc
    // if (!(isSuccess = CreateSemaphore("faucet", 2)))
    //     return 1;
    // // semaphore quan ly doc xuat file doc quyen
    // if (!(isSuccess = CreateSemaphore("file_mutex"), 0))
    //     return 1;
    // //TODO: read in file
    // nTest = 1;
    // n = 4;
    // a[0] = 5; a[1] = 3; a[2] = 4; a[3] = 1;
    int fInput;
    fInput = Open("input.txt", 0);
    if (fInput == -1) { 
        PrintString("\nOpen file failed!\n");
        return 1;
    }
    readStatus = ReadInt(fInput, &nTest);
    
    PrintNum(nTest);
    PrintNum(readStatus);


    Exit(0);
}