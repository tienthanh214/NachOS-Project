#include "pcb.h"
#include "utility.h"
#include "main.h"
#include "thread.h"
#include "addrspace.h"

//extern void StartProcess_2(void* arg);

void StartProcess_2(void *arg)
{
    // Lay fileName cua process id nay
    int *idPtr = (int *)arg;
    char *fileName = kernel->pTab->GetFileName(*idPtr);

    AddrSpace *space;
    space = new AddrSpace();
    space->Load(fileName);

    if (space == NULL)
    {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }

    kernel->currentThread->space = space;

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    kernel->machine->Run(); // jump to the user progam
    ASSERT(FALSE);          // machine->Run never returns;
                            // the address space exits
                            // by doing the syscall "exit"
}

PCB::PCB(int id)
{
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

PCB::~PCB()
{
    if (joinsem != NULL)
        delete this->joinsem;
    if (exitsem != NULL)
        delete this->exitsem;
    if (multex != NULL)
        delete this->multex;
    if (thread != NULL)
    {
        thread->FreeSpace();
        thread->Finish();
    }
}

int PCB::GetID() { return this->thread->processID; }

int PCB::GetNumWait() { return this->numwait; }

int PCB::GetExitCode() { return this->exitcode; }

void PCB::SetExitCode(int ec) { this->exitcode = ec; }

void PCB::JoinWait()
{
}

void PCB::JoinRelease()
{
}

void PCB::ExitWait()
{
}

void PCB::ExitRelease()
{
}

void PCB::IncNumWait()
{
    multex->P();
    ++numwait;
    multex->V();
}

void PCB::DecNumWait()
{
    multex->P();
    if (numwait > 0)
        --numwait;
    multex->V();
}

void PCB::SetFileName(char *fn) { strcpy(filename, fn); }

char *PCB::GetFileName() { return this->filename; }

int PCB::Exec(char *filename, int pid)
{
    // Goi mutex->P() de tranh nap 2 tien trinh cung 1 thoi diem
    multex->P();
    this->thread = new Thread(filename);
    // Neu thread tao khong thanh cong thi goi mutex->V()
    if (this->thread == NULL)
    {
        printf("\nPCB::Exec:: Not enough memory..!\n");
        multex->V();
        return -1;
    }
    // Dat processID cua thread nay la id.
    this->thread->processID = pid;
    // Dat parrentID cua thread này la processID cua thread goi thuc thi Exec
    this->parentID = kernel->currentThread->processID;
    // Goi thuc thi
    this->thread->Fork(StartProcess_2, (void *)pid);
    multex->V();
    // Tra ve id
    return pid;
}