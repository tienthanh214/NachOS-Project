#include "pcb.h"

#include "addrspace.h"
#include "main.h"
#include "thread.h"
#include "utility.h"

// extern void StartProcess_2(void* arg);

void StartProcess_2(int id) {
    char *fileName = kernel->pTab->GetFileName(id);
    AddrSpace *space;
    space = new AddrSpace();
    
    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddrSpace.");
        return;
    }
    if (space->Load(fileName)) {
        space->Execute();
        ASSERTNOTREACHED();
    }
    ASSERT(FALSE);
}

PCB::PCB(int id) {
    if (id == 0)
        this->parentID = -1;
    else
        this->parentID = kernel->currentThread->processID;
    this->numwait = this->exitcode = this->isBG = 0;
    this->thread = NULL;
    this->joinsem = new Semaphore("joinsem", 0);
    this->exitsem = new Semaphore("exitsem", 0);
    this->multex = new Semaphore("multex", 1);
}

PCB::~PCB() {
    if (joinsem != NULL)
        delete this->joinsem;
    if (exitsem != NULL)
        delete this->exitsem;
    if (multex != NULL)
        delete this->multex;
    if (thread != NULL) {
        thread->FreeSpace();
        thread->Finish();
    }
}

int PCB::GetID() { return this->thread->processID; }

int PCB::GetNumWait() { return this->numwait; }

int PCB::GetExitCode() { return this->exitcode; }

void PCB::SetExitCode(int ec) { this->exitcode = ec; }

// chuyen tien trinh sang trang thai block
// cho JoinRelease de duoc thuc thi tiep
void PCB::JoinWait() {
    joinsem->P();
}
// giai phong tien trinh 
void PCB::JoinRelease() {
    joinsem->V();
}
// block tien trinh cho den khi duoc cho phep exit
void PCB::ExitWait() {
    exitsem->P();
}
// tien trinh duoc phep exit
void PCB::ExitRelease() {
    exitsem->V();
}

void PCB::IncNumWait() {
    multex->P();
    ++numwait;
    multex->V();
}

void PCB::DecNumWait() {
    multex->P();
    if (numwait > 0)
        --numwait;
    multex->V();
}

void PCB::SetFileName(char *fn) { strcpy(filename, fn); }

char *PCB::GetFileName() { return this->filename; }

int PCB::Exec(char *filename, int pid) {
    // Goi mutex->P() de tranh nap 2 tien trinh cung 1 thoi diem
    multex->P();
    this->thread = new Thread(filename);
    // Neu thread tao khong thanh cong thi goi mutex->V()
    if (this->thread == NULL) {
        multex->V();
        return -1;
    }
    // Dat processID cua thread nay la id.
    this->thread->processID = pid;
    // Dat parrentID cua thread này la processID cua thread goi thuc thi Exec
    this->parentID = kernel->currentThread->processID;
    // Goi thuc thi
    this->thread->Fork((VoidFunctionPtr)&StartProcess_2, (void*)pid);
    multex->V();
    // Tra ve id
    return pid;
}