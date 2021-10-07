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

bool isDigit(char c) {
    if (c >= '0' && c <= '9')
        return true;
    return false;
}


int SysReadNum() {
    int num = 0;
    char c;
    bool isSign = false;

    do {
        c = kernel->synchConsoleIn->GetChar();
    } while (!isDigit(c) && c != '-');

    if (c == '-') {
        isSign = true;
        c = kernel->synchConsoleIn->GetChar();
    }

    do {
        num = num * 10 + c - '0';
        c = kernel->synchConsoleIn->GetChar();
    } while (isDigit(c));

    if (!isDigit(c))
        return 0;

    if (isSign) num = -num;

    return num;
}

void SysPrintNum(int op1) {
    if (op1 == 0) {
        kernel->synchConsoleOut->PutChar('0');
        return;
    }

    if (op1 < 0) {
        kernel->synchConsoleOut->PutChar('-');
        op1 = -op1;
    }

    char arr[11];
    int i = 0;
    int j, r;

    while (op1 != 0) {
        r = op1 % 10;
        arr[i] = r;
        i++;
        op1 = op1 / 10;
    }

    for (j = i - 1; j > -1; --j) {
        kernel->synchConsoleOut->PutChar('0' + arr[j]);
    }
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