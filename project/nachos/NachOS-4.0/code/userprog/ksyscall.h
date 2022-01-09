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

/* Xu ly syscall PrintString
    Input: buffer de in ra mna hinh, length kich thuoc chuoi 
*/
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
    buffer[idx++] = '\0';
    return idx;
}

/*  Xu ly syscall PrintString
    Input: buffer (char*) de in ra man hinh
*/
void SysPrintString(char *buffer)
{
    int length = 0;
    while (buffer[length]) // loop until meet '\0'
    {
        kernel->synchConsoleOut->PutChar(buffer[length++]);
    }
}

// Syscall tao file
void SysCreateFile(char *filename)
{
    if (strlen(filename) == 0)
    {
        printf("File name is not valid\n");
        DEBUG('a', "File name is not valid\n");
        kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
        return;
    }
    //Neu khong doc duoc
    if (filename == NULL)
    {
        printf("\n Not enough memory in system\n");
        DEBUG('a', "\n Not enough memory in system\n");
        kernel->machine->WriteRegister(2, -1);
        return;
    }
    //Tao file bang ham Create cua fileSystem, tra ve ket qua
    if (!kernel->fileSystem->Create(filename))
    {
        //Tao file that bai
        printf("\n Error create file '%s'", filename);
        kernel->machine->WriteRegister(2, -1);
        return;
    }
    kernel->machine->WriteRegister(2, 0);
    return;
}

/*  Xu ly syscall Exec
    Input: buffer (char*) ten cua process
*/
void SysExec(char *name)
{
    // Khi name == NULL, thong bao loi
    if (name == NULL)
    {
        DEBUG('a', "\n Name can not be NULL");
        printf("\n Name can not be NULL");
        kernel->machine->WriteRegister(2, -1);
        return;
    }
    // Mo mot file moi
    OpenFile *oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL)
    {
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
int SysJoin(int pid)
{
    return kernel->pTab->JoinUpdate(pid);
}

/* Xu ly syscall Exit
    Input: exit code
*/
void SysExit(int ec)
{
    kernel->pTab->ExitUpdate(ec);
    kernel->currentThread->FreeSpace();
    kernel->currentThread->Finish();
}
// Xu li syscall Open file
OpenFileID SysOpen(char *filename, int type)
{
    int pid = kernel ->currentThread->processID;
    return kernel->pTab->Open(pid,filename,type);
}
// Xu li syscall Close file
int SysClose(int id)
{
    int pid = kernel ->currentThread->processID;
    return kernel->pTab->Open(pid,id);
}

/* Xu ly syscall CreateSemaphore
    input: semaphore name, value
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysCreateSemaphore(char *name, int semVal)
{
    if (name == NULL)
    {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Create(name, semVal);
    if (res == -1)
    {
        printf("\n Can't create semaphore.");
        return -1;
    }
    return res;
}

/* Xu ly syscall Wait
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysWait(char *name)
{
    if (name == NULL)
    {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Wait(name);
    if (res == -1)
    {
        printf("\nSemaphore %s doesn't exist.", name);
        return -1;
    }
    return res;
}

/* Xu ly syscall Signal
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysSignal(char *name)
{
    if (name == NULL)
    {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        return -1;
    }
    int res = kernel->semTab->Signal(name);
    if (res == -1)
    {
        printf("\nSemaphore %s doesn't exist.", name);
        return -1;
    }
    return res;
}

/* Xu ly syscall Read
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysRead(char *buffer, int size, OpenFileId id)
{
    int pid = kernel ->currentThread->processID;
    return kernel->pTab->Read(pid,buffer,size,id);
}

/* Xu ly syscall Write
    input: semaphore name
    output -1 neu loi nguoc lai tra ve id cua semaphore
*/
int SysWrite(char *buffer, int size, OpenFileId id)
{
    int pid = kernel ->currentThread->processID;
    return kernel->pTab->Write(pid,buffer,size,id);
}

/* Xu ly syscall Seek
    input: position vi tri can di chuyen, id la id cua file
    output -1 neu loi nguoc lai tra ve pos vua di chuyen toi
*/
int SysSeek(int position, int id)
{
    // Id cua file nam ngoai vung quan li
    if (id < 0 || id > 9)
        return -1;
    // File ko ton tai thi bao loi
    if (kernel->fileSystem->openf[id] == NULL)
        return -1;
    // Khong the seek tren console
    if (id == 1 || id == 0)
        return -1;
    // Neu position la -1 thi seek het toan bo file
    position = (position == -1) ? kernel->fileSystem->openf[id]->Length() : position;
    if (position > kernel->fileSystem->openf[id]->Length() || position < 0)
        return -1;
    // Neu mo duoc file thi di chuyen den vi tri duoc yeu cau
    kernel->fileSystem->openf[id]->Seek(position);
    return position;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
