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
    Semaphore *joinsem; // semaphore cho qua trinh join
    Semaphore *exitsem; // semaphore cho qua trinh join
    Semaphore *multex;  // semaphore cho qua trinh join

    Thread *thread;     // Tien trinh
    char filename[MAX_FILENAME_LENGTH]; // Ten tien trinh

    int exitcode;
    int numwait;        // So tien trinh da join

   public:
    int parentID;       // ID cua tien trinh
    OpenFile** fileTable;
    int index;
    
    PCB();
    PCB(int id);
    ~PCB();

    int Exec(char *filename, int pid);  // Tao 1 thread moi
    int GetID();	
    int GetNumWait();

    void JoinWait();	// Tien trinh cha doi tien trinh con ket thuc
    void ExitWait();	// Tien trinh con xin ket thuc
    void JoinRelease();	// Tien trinh cha tiep tuc thuc thi
    void ExitRelease();	// Cho phep tien trinh con ket thuc

    void IncNumWait();  // Tang so tien trinh cho
    void DecNumWait();  // Giam so tien tring cho

    void SetExitCode(int ec);   // Dat exitcode cho ca tien trinh
    int GetExitCode();          // Lay exitcode cho ca tien trinh

    void SetFileName(char *fn); // Dat filename cho ca tien trinh
    char *GetFileName();        // Lay filename cho ca tien trinh

    int FindFreeSlot();         // Tim vi tri con trong de luu file can open
    OpenFileID Open(char*name, int type);
    int Close(OpenFileID fid);
    int Read(char* buffer, int charcount, OpenFileID id);
    int Write(char* buffer, int charcount, OpenFileID id);
    int Seek(int position, OpenFileID id);
};

#endif