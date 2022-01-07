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
    bool isSign, isInteger, isInput;
    *num = 0;
    isSign = false;
    isInteger = true;
    // Xoa cac dau cach va xuong dong o dau
    do
    {
        err = Read(&c, 1, id);
        if (err == END_OF_FILE) 
            return END_OF_FILE;
    } while (c == '\n' || c == ' ');
    // Xu li so am
    if (c == '-')
    {
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
    do
    {
        *num = (*num) * 10 + (c - '0');
        err = Read(&c, 1, id);
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ' && err != END_OF_FILE);
    // Thay doi gia tri so khi can
    if (!isInteger) *num = 0;
    if (isSign)     *num = -(*num);
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

/* void WriteNum(file, int num);
    ghi num vao file
*/

#endif