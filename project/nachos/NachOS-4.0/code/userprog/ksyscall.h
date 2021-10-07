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


void SysHalt() {
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
    return op1 + op2;
}

unsigned int SysRandomNumber() {
    RandomInit(time(0));
    return RandomNumber();
}

void SysReadString(char* buffer, int length) {
    int idx;
    char ch;
    for (idx = 0; idx < length; ++idx)
        buffer[idx] = 0;
    for (idx = 0; idx < length;) {
        do {
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\001' || ch == '\n')
            break;
        buffer[idx++] = ch;
    }
}

void SysPrintString(char* buffer) {
    int length = 0;
    while (buffer[length]) {
        kernel->synchConsoleOut->PutChar(buffer[length++]);
    }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
