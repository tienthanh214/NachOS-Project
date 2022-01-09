#include "syscall.h"

int main()
{
    char *buf;
    int i, cnt, id;

    PrintString("Console input: ");
    cnt = Read(buf, 50, 0);
    PrintString("Number of bytes read: ");
    PrintNum(cnt);
    PrintChar('\n');

    PrintString("Output: ");
    cnt = Write(buf, 50, 1);
    PrintString("Number of bytes write: ");
    PrintNum(cnt);
    PrintChar('\n');

    PrintString("File input: ");
    id = Open("test1.txt", 0);
    cnt = Read(buf, 50, id);
    PrintString("Number of bytes read: ");
    PrintNum(cnt);
    PrintChar('\n');

    PrintString("Output: ");
    cnt = Write(buf, 50, 1);
    PrintString("Number of bytes write: ");
    PrintNum(cnt);
    PrintChar('\n');

    Close(0);

    Halt();
    /* not reached */
}
