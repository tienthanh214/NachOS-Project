#include "syscall.h"

int main()
{
    char* buf;
    int i, cnt;
    
    cnt  = Read(buf, 50, 0);
    PrintNum(cnt);
    PrintChar('\n');
    PrintString(buf);

    Close(0);

    Halt();
    /* not reached */
}
