/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"
#include "fileutils.h"

int main()
{
    int id, i, cnt, err;
    err = 0;
    // CreateFile("test.txt");
    // -2 la het file
    // -3 la het dong

    id = Open("test.txt", 0);
    while (err != -2)
    {
        err = ReadInt(id, &cnt);
        PrintNum(cnt);
        PrintChar(' ');
        if (err == -3)
        {
            PrintChar('\n');
        }
        
    }

    Close(id);

    Halt();
    /* not reached */
}
