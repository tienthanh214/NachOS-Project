/*  ptable.h
    Data structure for manage process
        handle system call
*/

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAX_PROCESS 10

class PCB;
class Semaphore;

class PTable {
private:
    Bitmap *bm;             // Danh dau cac vi tri da su dung
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmsem;       // Semaphore de ngan chan
                            //   2 tien trinh nap cung luc
public:
    PTable(int size);
    ~PTable();


    void ExitUpdate(int ec);    // Xu li syscall SC_Exit
    int ExecUpdate(char *name); // Xu li syscall SC_Exec
    int JoinUpdate(int id);     // XU li syscall SC_Join

    bool IsExist(int pid);      // Kiem tra xem co ton tai chua
    int GetFreeSlot();          // Tim mot slot de luu tien trinh
    void Remove(int pid);       // Xoa thong tin tien trinh
    char *GetFileName(int id);  // Tra ve ten tien trinh

    void initStartProcess(char* name);  // Khoi chay tien trinh
   
    OpenFileID Open(int pid, char*name, int type);  // mo file name o tien trinh pid
    int Close(int pid, OpenFileID fid); // dong file fid o tien trinh pid
    int Read(int pid, char* buffer,int charcount, OpenFileID fid);      // doc file o tien trinh pid
    int Write(int pid, char* buffer, int charcount, OpenFileID fid);    // ghi file o tien trinh pid
    int Seek(int pid, int position, OpenFileID fid);    // seek file fid o tien trinh pid
};

#endif