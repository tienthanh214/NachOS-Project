/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
   
    CreateFile("test.txt");
    //
  
    PrintNum(Open("test.txt",0));
    Close(Open("test.txt",0));
    PrintString("closed file");
    Halt();

    /* not reached */
}
