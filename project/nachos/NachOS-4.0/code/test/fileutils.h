#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "stdbool.h"
#include "syscall.h"

#define HEAD_OF_LINE 1
#define END_OF_FILE -2
#define END_OF_LINE -3

bool isDigit(char c) {
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

int ReadInt(int id, int *num) {
    int err;
    char c;
    bool isSign, isInteger, isInput;
    *num = 0;
    isSign = false;
    isInteger = true;
    // Xoa cac dau cach va xuong dong o dau
    do {
        err = Read(&c, 1, id);
        if (err == END_OF_FILE)
            return END_OF_FILE;
    } while (c == '\n' || c == ' ');
    // Xu li so am
    if (c == '-') {
        isSign = true;
        err = Read(&c, 1, id);
        if (err == END_OF_FILE)
            return END_OF_FILE;
        if (c == '\n')
            return END_OF_LINE;
        if (c == ' ')
            return HEAD_OF_LINE;
    }
    // Doc chuoi ki tu va chuyen thanh so nguyen
    if (!isDigit(c) && c != '\n' && c != ' ')
        isInteger = false;
    do {
        *num = (*num) * 10 + (c - '0');
        err = Read(&c, 1, id);
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ' && err != END_OF_FILE);
    // Thay doi gia tri so khi can
    if (!isInteger) *num = 0;
    if (isSign) *num = -(*num);
    // Kiem tra vi tri so
    if (err == END_OF_FILE)
        // Truong hop file ket thuc ma ko xuong dong
        // thi xem ket thuc file la ket thuc dong
        return END_OF_LINE;
    if (c == '\n')
        // Truong hop so nam o cuoi dong
        return END_OF_LINE;
    return HEAD_OF_LINE;
}

void WriteChar(int fileid, char c) {  // ghi mot ki tu
    Write(&c, 1, fileid);
    return;
}

int countdigit(int n) {  // dem so chu so
    int count = 1;
    while (n >= 10) {
        n = n / 10;
        count++;
    }
    return count;
}

int firstDigit(int n)  // chu so dau tien
{
    while (n >= 10) {
        n = n / 10;
    }
    return n;
}

int pow10(int a) {  // luy thua 10
    int i, r;
    r = 1;
    for (i = 0; i < a; i++) {
        r = r * 10;
    }
    return r;
}

void WriteNum(int fileid, int num) {  // ghi mot so
    int i, count, digit;
    char ch;
    count = countdigit(num);
    for (i = 0; i < count; i++) {  // ghi tung chu so tu trai sang phai
        digit = firstDigit(num);
        num = num - digit * pow10(count - i - 1);
        ch = digit + '0';
        WriteChar(fileid, ch);
    }
    return;
}

#endif