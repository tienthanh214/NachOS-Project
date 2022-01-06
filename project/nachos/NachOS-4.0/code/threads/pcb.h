/*	pcb.h
	Process Control Block
*/

#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

const int MAX_FILENAME_LENGTH = 32;

class Semaphore;

class PCB {
   private:
    Semaphore *joinsem;
    Semaphore *exitsem;
    Semaphore *multex;

    Thread *thread;
    char filename[MAX_FILENAME_LENGTH];

    int exitcode;
    int numwait;

   public:
    int parentID;
    bool isBG;
    PCB();
    PCB(int id);
    ~PCB();

    int Exec(char *filename, int pid);
    int GetID();	
    int GetNumWait();

    void JoinWait();	// tien trinh cha doi tien trinh con ket thuc
    void ExitWait();	// tien trinh con xin ket thuc
    void JoinRelease();	// tien trinh cha tiep tuc thuc thi
    void ExitRelease();	// cho phep tien trinh con ket thuc

    void IncNumWait();

    void DecNumWait();

    void SetExitCode(int ec);
    int GetExitCode();

    void SetFileName(char *fn);
    char *GetFileName();
};

#endif