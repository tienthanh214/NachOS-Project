/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
    char a[30];
    int result;
    int k = 0, tmp, i;
    
    result = Add(42, 223);
    result = RandomNum();

    PrintString("\nEnter a string: ");
    ReadString(a, 20);
    PrintString("Your string: ");
    PrintString(a);
    PrintString("\n");
    Halt();

    /* not reached */
}
