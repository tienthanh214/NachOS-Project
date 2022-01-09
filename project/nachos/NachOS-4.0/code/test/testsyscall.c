#include"syscall.h"

void getB() {

}

int main(){
    char a[100];
    char b;
    int c;
    PrintString("\nEnter a String: ");
    ReadString(a,20);
    PrintString("Your String: ");
    PrintString(a);
    PrintString("\nEnter a character: ");
    b=ReadChar();
    PrintString("Your character: ");
    PrintChar(b);
    PrintString("\nEnter a Number: ");
    c= ReadNum();
    PrintString("Your Number: ");
    PrintNum(c);
    PrintString("\nA Random Number: ");
    c=RandomNum();
    PrintNum(c);
    PrintString("\n");
    
    Exit(0);
}