#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "syscall.h"
#include "stdbool.h"

#define HEAD_OF_LINE 1
#define END_OF_FILE -2
#define END_OF_LINE -3

bool isDigit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

int ReadInt(int id, int *num)
{
    int err;
    char c;
    bool isSign, isInteger;
    *num = 0;
    isSign = false;
    isInteger = true;
    // Xoa cac dau cach va xuong dong o dau
    do
    {
        err = Read(&c, 1, id);
        if (err == END_OF_FILE)
            return END_OF_FILE;
    } while (c == ' ' || c == '\n');
    // Xu li so am
    if (c == '-')
    {
        isSign = true;
        err = Read(&c, 1, id);
        if (err == END_OF_FILE)
            return END_OF_FILE;
        if (c == '\n' || c == ' ')
            *num = 0;
    }
    // Doc chuoi ki tu va chuyen thanh so nguyen
    if (!isDigit(c) && c != '\n' && c != ' ')
        isInteger = false;
    do
    {
        *num = (*num) * 10 + (c - '0');
        err = Read(&c, 1, id);
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ' && err != END_OF_FILE);
    // Xoa cac dau o cuoi
    if (!isInteger) { *num = 0; }
    if (isSign)     { *num = -(*num); }
    // Kiem tra vi tri so
    if (err == END_OF_FILE)
        return END_OF_FILE;
    if (c == '\n')
        return END_OF_LINE;
    // Xoa dau cach o cuoi xem co end line hay end file ko
    // do
    //     err = Read(&c, 1, id);
    // while (c == ' ');
    // // Kiem tra vi tri so
    // if (c == '\n')
    //     return END_OF_LINE;
    // if (err == END_OF_FILE)
    //     return END_OF_FILE;
    return HEAD_OF_LINE;
}

/* void WriteNum(file, int num);
    ghi num vao file
*/

#endif