/* stable.h
    Data structure for manage semaphore
*/

/*  --------------------------------
    19CNTN - HCMUS
    19120036, 19120176, 19120200
    --------------------------------
*/

#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "synch.h"

const unsigned int MAX_SEMAPHORE = 10;

class Semaphore;

class Sem {
private:
    char *name;
    Semaphore *sem;

public:
    Sem(char *semName, int value);
    ~Sem();

    char *getName();

    void wait();   // thuc hien thao tac cho
    void signal(); // thuc hien thao tac giai phong
};

class STable
{
private:
    Bitmap *mSemBitMap;
    Sem *semTable[MAX_SEMAPHORE];

public:
    STable();
    ~STable();

    int Create(char *name, int value); // tao 1 semaphore "name" neu chua ton tai ten
    int Wait(char *name);              // semaphore "name" wait
    int Signal(char *name);            // semaphore "name" giai phong
    int FindFreeSlot();                // tim slot trong trong STable
};

#endif // SCHEDULER_H
