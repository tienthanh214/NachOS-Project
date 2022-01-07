/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main()
{
    char* buf;
    int id, i, cnt;
    
    // CreateFile("test.txt");

    id = Open("test.txt", 0);
    PrintNum(id);
    cnt = Read(buf, 50, id);
    PrintNum(cnt);
    PrintChar('\n');
    PrintString(buf);
    

    Close(id);

    Halt();
    /* not reached */
}
