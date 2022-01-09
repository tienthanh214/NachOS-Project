/*
    ||          19CNTN - HCMUS
    || 19120036 - 19120176 - 19120200

    Bai toan dong bo:
        Co 1 voi nuoc va N sinh vien, moi sinh vien muon lay 10 lit nuoc
        Tai moi thoi diem chi co 1 sinh vien lay nuoc,
        Moi lan chi duoc lay 1 lit, va phai tra lai voi nuoc sau do
*/
/*
    main.c
    Tien trinh chinh goi thuc thi cac process Sinh vien (student)
*/

#include "syscall.h"

void ReadInt(int fileId, int *num);

int main() {
    int fInput, fOutput;
    int nStudent, i;
    int studentPid[10]; // process id cua cac tien trinh sinh vien
    // semaphore quan ly voi nuoc
    if (CreateSemaphore("faucet", 1) == -1) {
        PrintString("\nCan't create semaphore\n");
        Exit(1);
    }
    // doc N tu input.txt
    fInput = Open("input.txt", 1);
    ReadInt(fInput, &nStudent);
    Close(fInput);
    // tao file output.txt de ghi ket qua
    CreateFile("output.txt");
    // thuc thu N tien trinh sinh vien
    for (i = 0; i < nStudent; ++i) {
        studentPid[i] = Exec("student");
    }
    // join doi den khi cac tien trinh sinh vien ket thuc
    for (i = 0; i < nStudent; ++i) {
        Join(studentPid[i]);
    }
    // dung tien trinh chinh
    Exit(0);
}
// doc so nguyen
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