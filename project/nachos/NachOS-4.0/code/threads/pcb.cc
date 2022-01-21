#include "pcb.h"

#include "addrspace.h"
#include "main.h"
#include "thread.h"
#include "utility.h"
#include "kernel.h"
#include "synchconsole.h"
#include "syscall.h"

void StartProcess(int id)
{
    char *fileName = kernel->pTab->GetFileName(id);
    // Tao ra khong gian dia chi moi
    AddrSpace *space;
    space = new AddrSpace();
    // Bao loi neu khong tao thanh cong
    if (space == NULL)
    {
        printf("\nError in StartProcess: Can't create AddrSpace.");
        return;
    }
    if (space->Load(fileName))
    {
        space->Execute();
        ASSERTNOTREACHED();
    }
    ASSERT(FALSE);
}

PCB::PCB(int id)
{
    // Khoi tao cac phan tu
    if (id == 0)
        this->parentID = -1;
    else
        this->parentID = kernel->currentThread->processID;
    this->numwait = this->exitcode = 0;
    this->thread = NULL;
    this->joinsem = new Semaphore("joinsem", 0);
    this->exitsem = new Semaphore("exitsem", 0);
    this->multex = new Semaphore("multex", 1);
    fileTable = new OpenFile *[10];
    index = 0;
    for (int i = 0; i < 10; ++i)
    {
        fileTable[i] = NULL;
    }
    kernel->fileSystem->Create("stdin", 0);
    kernel->fileSystem->Create("stdout", 0);
    fileTable[index++] = kernel->fileSystem->Open("stdin", 2);
    fileTable[index++] = kernel->fileSystem->Open("stdout", 3);
}

PCB::~PCB()
{
    // Giai phong cac bo nho da cap phat
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
    for (int i = 0; i < 10; ++i)
    {
        if (fileTable[i] != NULL)
            delete fileTable[i];
    }
    delete[] fileTable;
}

int PCB::GetID() { return this->thread->processID; }
int PCB::GetNumWait() { return this->numwait; }
int PCB::GetExitCode() { return this->exitcode; }
void PCB::SetExitCode(int ec) { this->exitcode = ec; }

// chuyen tien trinh sang trang thai block
// cho JoinRelease de duoc thuc thi tiep
void PCB::JoinWait()
{
    joinsem->P();
}
// giai phong tien trinh
void PCB::JoinRelease()
{
    joinsem->V();
}
// block tien trinh cho den khi duoc cho phep exit
void PCB::ExitWait()
{
    exitsem->P();
}
// tien trinh duoc phep exit
void PCB::ExitRelease()
{
    exitsem->V();
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
    if (numwait > 0) // numwait luon lon hon hoac bang 0
        --numwait;
    multex->V();
}

void PCB::SetFileName(char *fn) { strcpy(filename, fn); }

char *PCB::GetFileName() { return this->filename; }

int PCB::Exec(char *filename, int pid)
{
    // Tranh nap 2 tien trinh cung 1 thoi diem
    multex->P();
    this->thread = new Thread(filename);
    // Neu thread tao khong thanh cong thi goi mutex->V() va tra ve -1
    if (this->thread == NULL)
    {
        multex->V();
        printf("\nError in PCB::Exec: Can't create AddrSpace.");
        return -1;
    }
    // Dat processID cua thread nay la id.
    this->thread->processID = pid;
    // Dat parrentID cua thread này la processID cua thread goi thuc thi Exec
    this->parentID = kernel->currentThread->processID;
    // Goi thuc thi
    this->thread->Fork((VoidFunctionPtr)&StartProcess, (void *)pid);
    multex->V();
    // Tra ve id
    return pid;
}

// ---- file table quan ly file ----

int PCB::FindFreeSlot()
{
    for (int i = 2; i < 10; i++)
    {
        if (this->fileTable[i] == NULL)
            return i;
    }
    return -1;
}

OpenFileID PCB::Open(char *name, int type)
{
    for (int i = 0; i < 10; ++i)
    {
        if (fileTable[i] == NULL)
            continue;
        char *openName = fileTable[i]->GetFilename();
        if (openName != NULL && strcmp(openName, name) == 0)
        {
            printf("\nError in PCB::Open: name invalid.");
            return -1;
        }
    }
    int freeSlot = this->FindFreeSlot();
    if (freeSlot == -1) return -1;
    //Chi xu li khi con slot trong
    if (type == 0 || type == 1) { //chi xu li khi type = 0 hoac 1
        fileTable[freeSlot] = kernel->fileSystem->Open(name, type);
        if (fileTable[freeSlot] != NULL) // Da mo file thanh cong
            return freeSlot;
    }
    else if (type == 2) // xu li stdin voi type = 2
        return 0;
    else // xu li stdout voi type = 3
        return 1;
    return -1;

}

// Xu li syscall Close file
int PCB::Close(OpenFileID id)
{
    if (id >= 0 && id <= 9) //Chi xu li khi file id nam trong [0, 9]
    {
        if (fileTable[id]) //neu da mo file thanh cong
        {
            delete fileTable[id]; //Xoa vung nho luu tru file
            fileTable[id] = NULL; //Gan vung nho NULL
            return 0;
        }
    }
    return -1;
}

int readString(char *buffer, int length)
{
    int idx;
    char ch;
    for (idx = 0; idx < length; ++idx)
        buffer[idx] = 0;
    for (idx = 0; idx < length;)
    {
        do
        { // bo qua cac ki tu EOF
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[idx++] = ch;
    }
    buffer[idx + 1] = '\0';
    return idx;
}

int PCB::Read(char *buffer, int size, OpenFileID id)
{
    int oldPos, newPos;
    // Id cua file nam ngoai vung quan li
    if (id < 0 || id > 9)
    {
        printf("\nError in PCB::Read: file id out of range.");
        return -1;
    }
    // Doc trong stdout
    if (id == CONSOLE_OUTPUT)
    {
        printf("\nError in PCB::Read: cannot read in console output.");
        return -1;
    }
    // File ko ton tai thi bao loi
    if (fileTable[id] == NULL)
    {
        printf("\nError in PCB::Read: file is null.");
        return -1;
    }
    // Lay vi tri ban dau
    oldPos = fileTable[id]->GetCurrentPos();
    if (id == CONSOLE_INPUT)
    {
        int actualSize = readString(buffer, size);
        return actualSize;
    }
    if ((fileTable[id]->Read(buffer, size)) > 0)
    {
        // Lay vi tri moi
        newPos = fileTable[id]->GetCurrentPos();
        return newPos - oldPos;
    }
    return -2;
}

int PCB::Write(char *buffer, int size, OpenFileID id)
{
    int oldPos, newPos;
    // Id cua file nam ngoai vung quan li
    if (id < 0 || id > 9)
    {
        printf("\nError in PCB::Write: file id out of range.");
        return -1;
    }
    // Ghi trong stdin
    if (id == CONSOLE_INPUT)
    {
        printf("\nError in PCB::Write: cannot write in console input.");
        return -1;
    }
    // File ko ton tai thi bao loi
    if (fileTable[id] == NULL)
    {
        printf("\nError in PCB::Write: file is null.");
        return -1;
    }
    // Neu la file read only thi bao loi
    if (fileTable[id]->type == 1)
    {
        printf("\nError in PCB::Write: file is read only.");
        return -1;
    }
    // Lay vi tri ban dau
    oldPos = fileTable[id]->GetCurrentPos();
    if (id == CONSOLE_OUTPUT)
    {
        int i = 0;
        while (buffer[i] != 0 && buffer[i] != '\n' && i < size)
        {
            // Ghi tung ki tu va dem so ki tu
            kernel->synchConsoleOut->PutChar(buffer[i]);
            i++;
        }
        // Them ki tu xuong dong o cuoi
        buffer[i] = '\n';
        kernel->synchConsoleOut->PutChar(buffer[i]);
        return i;
    }
    if ((this->fileTable[id]->Write(buffer, size)) > 0)
    {
        // Lay vi tri moi
        newPos = fileTable[id]->GetCurrentPos();
        return newPos - oldPos;
    }
    return -1;
}

int PCB::Seek(int position, OpenFileID id)
{
    // Id cua file nam ngoai vung quan li
    if (id < 0 || id > 9)
        return -1;
    // File ko ton tai thi bao loi
    if (fileTable[id] == NULL)
        return -1;
    // Khong the seek tren console
    if (id == 1 || id == 0)
        return -1;
    // Neu position la -1 thi seek het toan bo file
    position = (position == -1) ? fileTable[id]->Length() : position;
    if (position > fileTable[id]->Length() || position < 0)
        return -1;
    // Neu mo duoc file thi di chuyen den vi tri duoc yeu cau
    fileTable[id]->Seek(position);
    return position;
}