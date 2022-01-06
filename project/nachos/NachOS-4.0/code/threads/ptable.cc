#include "ptable.h"
#include "main.h"
#include "openfile.h"

#define For(i, a, b) for(int i = (a); i < b; ++i)

PTable::PTable(int size)
{
    if (size < 0)
        return;
    psize = size;
    // Khoi tao bm va bmsem de su dung
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);
    For(i, 0, MAX_PROCESS)
    {
        pcb[i] = 0;
    }
    bm->Mark(0);
    pcb[0] = new PCB(0);
    pcb[0]->SetFileName("./test/scheduler");
    pcb[0]->parentID = -1;
}

PTable::~PTable()
{
    if (bm != 0)
        delete bm;

    For(i, 0, psize)
    {
        if (pcb[i] != 0)
            delete pcb[i];
    }

    if (bmsem != 0)
        delete bmsem;
}

int PTable::ExecUpdate(char *name)
{
    bmsem->P();

    if (name == NULL)
    {
        printf("\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }

    if (strcmp(name, "./test/scheduler") == 0 || strcmp(name, kernel->currentThread->getName()) == 0)
    {
        printf("\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();

    if (index < 0)
    {
        printf("\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    pcb[index]->parentID = kernel->currentThread->processID;
    int pid = pcb[index]->Exec(name, index);
    bmsem->V();

    return pid;
}

int PTable::JoinUpdate(int id)
{
    if (id < 0)
    {
        printf("\nPTable::JoinUpdate : id = %d", id);
        return -1;
    }

    if (kernel->currentThread->processID != pcb[id]->parentID)
    {
        printf("\nPTable::JoinUpdate Can't join in process which is not it's parent process.\n");
        return -1;
    }

    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();
    int ec = pcb[id]->GetExitCode();
    pcb[id]->ExitRelease();

    return ec;
}

int PTable::ExitUpdate(int exitcode)
{
    int id = kernel->currentThread->processID;
    if (id == 0)
    {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }

    if (IsExist(id) == false)
    {
        printf("\nPTable::ExitUpdate: This %d is not exist. Try again?", id);
        return -1;
    }

    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();
    Remove(id);
    return exitcode;
}

int PTable::GetFreeSlot()
{
    return bm->FindAndSet();
}

bool PTable::IsExist(int pid)
{
    return bm->Test(pid);
}

void PTable::Remove(int pid)
{
    // Xoa tien trinh tren bitmap
    bm->Clear(pid);
    // Xoa tien trinh tren mang quan li
    if (pcb[pid] != 0)
        delete pcb[pid];
}

char* PTable::GetFileName(int id)
{
    return (pcb[id]->GetFileName());
}