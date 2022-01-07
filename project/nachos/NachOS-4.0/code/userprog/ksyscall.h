/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "machine.h"
#include "filesys.h"

void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

/*  Xu ly syscall RandomNumber
    Output: sinh ngau nhieu so nguyen duong
*/
unsigned int SysRandomNumber()
{
    RandomInit(time(0)); // goi ham init random trong sysdep.cc void seed = time(0)
    return RandomNumber();
}

bool isDigit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

// Ham xu li cho system call ReadNum
int SysReadNum()
{
    int num = 0;
    char c;
    bool isSign = false;
    bool isInteger = true;

    do
    { // bo qua cac ki tu khoang trang va xuong dong
        c = kernel->synchConsoleIn->GetChar();
    } while (c == ' ' || c == '\n');

    // Xu li so am
    if (c == '-')
    {
        isSign = true;
        c = kernel->synchConsoleIn->GetChar();
        if (c == '\n' || c == ' ') // Tra ve 0 khi co dau cach sau dau '-'
            return 0;
    }

    // Neu chua ki tu khong phai so thi khong phai la so nguyen
    if (!isDigit(c))
        isInteger = false;

    // Chuyen chuoi ki tu thanh gia tri so
    do
    {
        num = num * 10 + c - '0';
        c = kernel->synchConsoleIn->GetChar();

        // Neu chua ki tu khong phai so thi so khong phai la so nguyen
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ');

    if (!isInteger) // Neu khong phai la so nguyen thi tra ve 0
        return 0;

    if (isSign) // Neu la so am
        num = -num;

    return num;
}

// Ham xu li cho system call PrintNum
void SysPrintNum(int op1)
{
    // Neu la so 0 thi in ra so 0
    if (op1 == 0)
    {
        kernel->synchConsoleOut->PutChar('0');
        return;
    }

    // Neu la so am thi in ra dau '-' truoc
    if (op1 < 0)
    {
        kernel->synchConsoleOut->PutChar('-');
        op1 = -op1;
    }

    char arr[10];
    int i = 0;
    int j, r;

    // Chuyen gia tri tuyet doi thanh mang ki tu
    while (op1 != 0)
    {
        r = op1 % 10;
        arr[i] = r;
        i++;
        op1 = op1 / 10;
    }

    // In mang ki tu cac so
    for (j = i - 1; j > -1; --j)
    {
        kernel->synchConsoleOut->PutChar('0' + arr[j]);
    }
}
// Xu ly syscall ReadChar
char SysReadChar()
{
    char c;
    c = kernel->synchConsoleIn->GetChar();
    return c;
}
// Xu ly syscall PrintChar
void SysPrintChar(char c)
{
    kernel->synchConsoleOut->PutChar(c);
}

/*  Xu ly syscall ReadString
    Input: buffer (char*), do dai toi da cua input
    Ket qua doc duoc tra vao buffer
*/
void SysReadString(char *buffer, int length)
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
        if (ch == '\001' || ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[idx++] = ch;
    }
}

/*  Xu ly syscall PrintString
    Input: buffer (char*) de in ra man hinh
*/
void SysPrintString(char *buffer)
{
    int length = 0;
    while (buffer[length]) { // loop until meet '\0'
        kernel->synchConsoleOut->PutChar(buffer[length++]);
    }
}

void SysCreateFile(char *filename)
{
    if (strlen(filename) == 0)
    {
        printf("\n File name is not valid");
        DEBUG('a', "\n File name is not valid");
        kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
        return;
    }
    if (filename == NULL) //Neu khong doc duoc
    {
        printf("\n Not enough memory in system");
        DEBUG('a', "\n Not enough memory in system");
        kernel->machine->WriteRegister(2, -1);
        return;
    }

    if (!kernel->fileSystem->Create(filename)) //Tao file bang ham Create cua fileSystem, tra ve ket qua
    {
        //Tao file that bai
        printf("\n Error create file '%s'", filename);
        kernel->machine->WriteRegister(2, -1);
        return;
    }
    kernel->machine->WriteRegister(2, 0);
    return;
}

void SysOpen(char *filename, int type)
{
    int freeSlot = kernel->fileSystem->FindFreeSlot();
    if (freeSlot != -1) //Chi xu li khi con slot trong
    {
        if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
        {
            if ((kernel->fileSystem->openf[freeSlot] = kernel->fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
            {
                kernel->machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
                return;
            }
        }
        else if (type == 2) // xu li stdin voi type quy uoc la 2
        {
            kernel->machine->WriteRegister(2, 0); //tra ve OpenFileID
            return;
        }
        else // xu li stdout voi type quy uoc la 3
        {
            kernel->machine->WriteRegister(2, 1); //tra ve OpenFileID
            return;
        }
        return;
    }
    kernel->machine->WriteRegister(2, -1); //Khong mo duoc file return -1
    return;
}

void SysClose(int id)
{
    if (id >= 0 && id <= 9) //Chi xu li khi fid nam trong [0, 14]
    {
        if (kernel->fileSystem->openf[id]) //neu mo file thanh cong
        {
            delete kernel->fileSystem->openf[id];  //Xoa vung nho luu tru file
            kernel->fileSystem->openf[id] = NULL; //Gan vung nho NULL
            kernel->machine->WriteRegister(2, 0);
            return;
        }
    }
    kernel->machine->WriteRegister(2, -1);
    return;
}

/*  Xu ly syscall Exec
    Input: buffer (char*) ten cua process
*/
void SysExec(char* name) {
    if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        kernel->machine->WriteRegister(2, -1);
        return;
    }

    OpenFile *oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL) {
        printf("\nExec:: Can't open this file.");
        kernel->machine->WriteRegister(2, -1);
        return;
    }
    delete oFile;
    int id = kernel->pTab->ExecUpdate(name);
    kernel->machine->WriteRegister(2, id);
}

/* Xu ly syscall Join
    Input: pid tien trinh cha join 
*/
int SysJoin(int pid) {
    return kernel->pTab->JoinUpdate(pid);
}

/* Xu ly syscall Exit
    Input: exit code
*/
void SysExit(int ec) {
    kernel->pTab->ExitUpdate(ec);
    kernel->currentThread->FreeSpace();
    kernel->currentThread->Finish();
}

/* Xu ly syscall CreateSemaphore
    input: semaphore name, value
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysCreateSemaphore(char* name, int semVal) {
    if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Create(name, semVal);
    if (res == -1) {
        printf("\n Can't create semaphore.");
        return -1;
    }
    return res;
}

/* Xu ly syscall Wait
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysWait(char* name) {
    if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Wait(name);
    if (res == -1) {
        printf("\nSemaphore %s doesn't exist.", name);
        return -1;
    }
    return res;
}

/* Xu ly syscall Signal
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysSignal(char* name) {
    if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Signal(name);
    if (res == -1) {
        printf("\nSemaphore %s doesn't exist.", name);
        return -1;
    }
    return res;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
