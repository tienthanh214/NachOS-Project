#include "pcb.h"
#include "utility.h"
#include "main.h"
#include "thread.h"
#include "addrspace.h"

extern void StartProcess_2(void *arg);

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

// Process tranlation to block

// Wait for JoinRelease to continue exec

void PCB::JoinWait()
{
}

void PCB::JoinRelease()
{
}

void PCB::ExitWait()
{
}

// Release wating process

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

int PCB::Exec(char *filename, int id)
{
    // Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
    multex->P();
    // Kiểm tra thread đã khởi tạo thành công chưa, nếu chưa thì báo lỗi là không đủ bộ nhớ, gọi mutex->V() và return.
    this->thread = new Thread(filename);
    if (this->thread == NULL)
    {
        printf("\nPCB::Exec:: Not enough memory..!\n");
        multex->V();
        return -1;
    }
    // Đặt processID của thread này là id.
    this->thread->processID = id;
    // Đặt parrentID của thread này là processID của thread gọi thực thi Exec
    this->parentID = kernel->currentThread->processID;
    // Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int, sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
    this->thread->Fork(StartProcess_2, (void *)id);
    multex->V();
    // Trả về id.
    return id;
}