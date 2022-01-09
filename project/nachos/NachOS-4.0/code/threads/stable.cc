/* stable.cc
    Implement data structure for manage semaphore
*/

#include "stable.h"

// --------- Sem implementation ---------
Sem::Sem(char *semName, int value) {
    this->name = new char[strlen(semName) + 1];
    strcpy(this->name, semName);
    this->sem = new Semaphore(this->name, value);
}

Sem::~Sem() {
    if (sem != NULL)
        delete sem;
    delete this->name;
}

char *Sem::getName() {
    return this->name;
}

void Sem::wait() {
    this->sem->P();
}

void Sem::signal() {
    this->sem->V();
}

// --------- STable implementation ----------
STable::STable() {
    this->mSemBitMap = new Bitmap(MAX_SEMAPHORE);
    for (unsigned int i = 0; i < MAX_SEMAPHORE; ++i) {
        semTable[i] = NULL;
    }
}

STable::~STable() {
    for (unsigned int i = 0; i < MAX_SEMAPHORE; ++i) {
        if (semTable[i] != NULL) {
            delete semTable[i];
            semTable[i] = NULL;
        }
    }
    if (mSemBitMap) {
        delete mSemBitMap;
        mSemBitMap = NULL;
    }
}
// Create, Wait, Signal
// tra ve -1 neu da ton tai "name"
// tra ve id trong array neu tao thanh cong
int STable::Create(char *name, int value) {
    for (unsigned int i = 0; i < MAX_SEMAPHORE; ++i) {
        if (mSemBitMap->Test(i)) {
            if (!strcmp(name, semTable[i]->getName())) {
                return -1;
            }
        }
    }
    int idx = this->FindFreeSlot();
    if (idx < 0)
        return -1;
    semTable[idx] = new Sem(name, value);
    return idx;
}

int STable::Wait(char *name) {
    for (unsigned int i = 0; i < MAX_SEMAPHORE; ++i) {
        if (mSemBitMap->Test(i)) {
            if (!strcmp(name, semTable[i]->getName())) {
                semTable[i]->wait();
                return 1;
            }
        }
    }
    return -1;
}

int STable::Signal(char *name) {
    for (unsigned int i = 0; i < MAX_SEMAPHORE; ++i) {
        if (mSemBitMap->Test(i)) {
            if (!strcmp(name, semTable[i]->getName())) {
                semTable[i]->signal();
                return 1;
            }
        }
    }
    return -1;
}

int STable::FindFreeSlot() {
    return mSemBitMap->FindAndSet();
}