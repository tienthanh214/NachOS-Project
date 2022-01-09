#include "syscall.h"

void ReadInt(int fileId, int *num);

int main() {
    int fInput, fOutput;
    int nStudent, i;
    int studentPid[10];
    // semaphore quan ly voi nuoc
    if (CreateSemaphore("faucet", 1) == -1) {
        PrintString("\nCan't create semaphore\n");
        Exit(1);
    }

    fInput = Open("input.txt", 1);
    ReadInt(fInput, &nStudent);
    Close(fInput);
    CreateFile("output.txt");

    for (i = 0; i < nStudent; ++i) {
        studentPid[i] = Exec("student");
    }

    for (i = 0; i < nStudent; ++i) {
        Join(studentPid[i]);
    }

    Exit(0);
}

void ReadInt(int fileId, int *num) {
    char c;
    do {
        Read(&c, 1, fileId);
    } while (!('0' <= c && c <= '9'));
    do {
        *num = (*num) * 10 + (c - '0');
        if (Read(&c, 1, fileId) != 1)
            break;
    } while ('0' <= c && c <= '9');
}