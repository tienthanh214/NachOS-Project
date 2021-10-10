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

// Ham xu li cho system call ReadNum
int SysReadNum() {
    int num = 0;
    char c;
    bool isSign = false;
    bool isInteger = true;

    do {    // ignore all white space
        c = kernel->synchConsoleIn->GetChar();
    } while (c == ' ' || c == '\n');

    if (c == '-') {
        isSign = true;
        c = kernel->synchConsoleIn->GetChar();
        if (c == '\n' || c == ' ')      // when negative sign then white space
            return 0;
    }

    if (!isDigit(c))
        isInteger = false;

    do {
        num = num * 10 + c - '0';
        c = kernel->synchConsoleIn->GetChar();

        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ');

    if (!isInteger)
        return 0;

    if (isSign)
        num = -num;

    return num;
}

// Ham xu li cho system call PrintNum
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

char SysReadChar() {
    char c;
    c = kernel->synchConsoleIn->GetChar();
    return c;
};

void SysPrintChar(char c) {
    kernel->synchConsoleOut->PutChar(c);
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
