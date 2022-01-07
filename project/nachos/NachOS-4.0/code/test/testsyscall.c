#include"syscall.h"

void getB() {

}

int main(){
    /* char a[100];
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
    int pingId, pongId, i;
    PrintString("Test multiprogramming\n\n");
    pingId = Exec("./test/ping");
    pongId = Exec("./test/pong");
    Join(pongId);
    Join(pingId);
    
    // PrintString("\n---- DONE -----\n"); */
    int pingId, pongId, i;
    int isSuccess;    
    PrintString("Test semaphore");

    PrintString("\n---- DONE -----\n");
    isSuccess = CreateSemaphore("ping", 0);
    // PrintNum(isSuccess);
    isSuccess = CreateSemaphore("pong", 0);
    // PrintNum(isSuccess);
 
    pingId = Exec("ping");
    pongId = Exec("pong");
    Join(pingId);
    Join(pongId);
    Exit(0);
}