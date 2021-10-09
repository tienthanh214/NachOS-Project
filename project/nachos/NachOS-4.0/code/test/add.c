/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
    // char a[30];
    // int result;
    // int k = 0, tmp, i;
    
    // result = Add(42, 223);
    // PrintNum(result);
    // result = RandomNum();
    // PrintChar('\n');
    // PrintNum(result);

    // // PrintChar('a');

    // PrintString("\nEnter a string: ");
    // ReadString(a, 20);
    // PrintString("Your string: ");
    // PrintString(a);
    // PrintString("\n");
    int x, y;
    char c, d;
    d = ReadChar();
    x = ReadNum();
    c = ReadChar();
    y = ReadNum();
    PrintChar(d);
    PrintNum(x); 
    PrintChar(c);
    PrintNum(y);
    Halt();

    /* not reached */
}
