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
    Bitmap *bm;
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmsem;

public:
    PTable(int size);
    ~PTable();


    int ExitUpdate(int ec);
    int ExecUpdate(char *name);
    int JoinUpdate(int id);

    bool IsExist(int pid);
    int GetFreeSlot();
    void Remove(int pid);
    char *GetFileName(int id);

    void initStartProcess(char* name);
};

#endif