#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "syscall.h"
#include "stdbool.h"

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

    do Read(&c, 1, id);
    while (c == ' ' || c == '\n');

    if (c == '-')
    {
        isSign = true;
        Read(&c, 1, id);
        if (c == '\n' || c == ' ') return 0;
    }

    if (!isDigit(c)) isInteger = false;

    do
    {
        *num = (*num) * 10 + (c - '0');
        err = (Read(&c, 1, id));
        
        if (c == '\n') err = END_OF_LINE;
        if (!isDigit(c) && c != '\n' && c != ' ')
            isInteger = false;
    } while (c != '\n' && c != ' ' && err != END_OF_FILE);

    if (!isInteger) return 0;
    if (isSign) *num = -(*num);

    return err;
}

/* void WriteNum(file, int num);
    ghi num vao file
*/

#endif