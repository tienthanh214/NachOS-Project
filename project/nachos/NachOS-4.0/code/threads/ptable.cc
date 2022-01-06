#include "ptable.h"

#include "main.h"
#include "openfile.h"

PTable::PTable(int size) {
    if (size < 0)
        return;
    if (size > MAX_PROCESS)
        size = MAX_PROCESS;
    psize = size;
    // Khoi tao bm va bmsem de su dung
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);
    for (int i = 0; i < psize; ++i)
        pcb[i] = 0;
    initStartProcess("./test/testsyscall");
}

void PTable::initStartProcess(char* name) {
    bm->Mark(0);
    pcb[0] = new PCB(0);
    pcb[0]->SetFileName(name);
    pcb[0]->parentID = -1;
}

PTable::~PTable() {
    if (bm != 0)
        delete bm;

    for (int i = 0; i < psize; ++i) {
        if (pcb[i] != 0)
            delete pcb[i];
    }

    if (bmsem != 0)
        delete bmsem;
}

int PTable::ExecUpdate(char *name) {
    bmsem->P();

    if (name == NULL) {
        printf("\nPTable::ExecUpdate : Can't execute name is NULL.\n");
        bmsem->V();
        return -1;
    }

    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        printf("\nPTable::Exec : Can't execute itself.\n");
        bmsem->V();
        return -1;
    }

    if (strcmp(name, pcb[0]->GetFileName()) == 0) {
        printf("\nPTable::ExecUpdate : Can't execute main process");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();

    if (index < 0) {
        printf("\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    pcb[index]->parentID = kernel->currentThread->processID;
    int pid = pcb[index]->Exec(name, index);
    // printf("ptable: %d %d %d\n", index, pcb[index]->parentID, pid);

    bmsem->V();

    return pid;
}

int PTable::JoinUpdate(int id) {
    if (id < 0) {
        printf("\nPTable::JoinUpdate : id = %d\n", id);
        return -1;
    }
    // chi cho phep join vao tien trinh cha 
    if (kernel->currentThread->processID != pcb[id]->parentID) {
        printf("\nPTable::JoinUpdate Can't join in process which is not it's parent process.\n");
        return -1;
    }

    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();    // cho den khi tien trinh con ket thuc
    int ec = pcb[id]->GetExitCode();
    pcb[id]->ExitRelease(); // cho phep tien trinh con duoc ket thuc

    return ec;
}

int PTable::ExitUpdate(int exitcode) {
    int id = kernel->currentThread->processID;
    // neu la main process thi Halt
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }
    // neu khong ton tai process id
    if (IsExist(id) == false) {
        printf("\nPTable::ExitUpdate: This %d is not exist. Try again?", id);
        return -1;
    }

    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();
    pcb[id]->JoinRelease(); // giai phong tien trinh
    pcb[id]->ExitWait();    // xin tien trinh cha cho exit
    Remove(id);
    return exitcode;
}

int PTable::GetFreeSlot() {
    return bm->FindAndSet();
}

bool PTable::IsExist(int pid) {
    return bm->Test(pid);
}

void PTable::Remove(int pid) {
    // Xoa tien trinh tren bitmap
    bm->Clear(pid);
    // Xoa tien trinh tren mang quan li
    if (pcb[pid] != 0)
        delete pcb[pid];
}

char *PTable::GetFileName(int id) {
    return (pcb[id]->GetFileName());
}