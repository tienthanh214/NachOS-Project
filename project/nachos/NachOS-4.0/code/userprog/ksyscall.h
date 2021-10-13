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

/*  Xu ly syscall RandomNumber
    Output: sinh ngau nhieu so nguyen duong
*/
unsigned int SysRandomNumber() {
    RandomInit(time(0));   // goi ham init random trong sysdep.cc void seed = time(0)
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

    do {    // bo qua cac ki tu khoang trang va xuong dong
        c = kernel->synchConsoleIn->GetChar();
    } while (c == ' ' || c == '\n');

    // Xu li so am
    if (c == '-') {
        isSign = true;
        c = kernel->synchConsoleIn->GetChar();
        if (c == '\n' || c == ' ')  // Tra ve 0 khi co dau cach sau dau '-'
            return 0;
    }

    // Neu chua ki tu khong phai so thi khong phai la so nguyen
    if (!isDigit(c))
        isInteger = false;

    // Chuyen chuoi ki tu thanh gia tri so
    do {
        num = num * 10 + c - '0';
        c = kernel->synchConsoleIn->GetChar();

        // Neu chua ki tu khong phai so thi so khong phai la so nguyen
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ');

    if (!isInteger) // Neu khong phai la so nguyen thi tra ve 0
        return 0;

    if (isSign)     // Neu la so am
        num = -num;

    return num;
}

// Ham xu li cho system call PrintNum
void SysPrintNum(int op1) {
    // Neu la so 0 thi in ra so 0
    if (op1 == 0) {
        kernel->synchConsoleOut->PutChar('0');
        return;
    }

    // Neu la so am thi in ra dau '-' truoc
    if (op1 < 0) {
        kernel->synchConsoleOut->PutChar('-');
        op1 = -op1;
    }

    char arr[10];
    int i = 0;
    int j, r;

    // Chuyen gia tri tuyet doi thanh mang ki tu
    while (op1 != 0) {
        r = op1 % 10;
        arr[i] = r;
        i++;
        op1 = op1 / 10;
    }

    // In mang ki tu cac so
    for (j = i - 1; j > -1; --j) {
        kernel->synchConsoleOut->PutChar('0' + arr[j]);
    }
}
// Xu ly syscall ReadChar
char SysReadChar() {
    char c;
    c = kernel->synchConsoleIn->GetChar();
    return c;
}
// Xu ly syscall PrintChar
void SysPrintChar(char c) {
    kernel->synchConsoleOut->PutChar(c);
}

/*  Xu ly syscall ReadString
    Input: buffer (char*), do dai toi da cua input
    Ket qua doc duoc tra vao buffer
*/
void SysReadString(char* buffer, int length) {
    int idx;
    char ch;
    for (idx = 0; idx < length; ++idx)
        buffer[idx] = 0;
    for (idx = 0; idx < length;) {
        do { // bo qua cac ki tu EOF
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\001' || ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[idx++] = ch;
    }
}

/*  Xu ly syscall PrintString
    Input: buffer (char*) de in ra man hinh
*/
void SysPrintString(char* buffer) {
    int length = 0;
    while (buffer[length]) { // loop until meet '\0'
        kernel->synchConsoleOut->PutChar(buffer[length++]);
    }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
