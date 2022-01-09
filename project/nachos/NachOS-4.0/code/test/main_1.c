/*  Bai toan dong bo:
        chuong trinh quan ly sinh vien voi nuoc
            "thong ke su dung may nong lanh"
        co 2 Voi nuoc va nhieu Sinh vien muon uong nuoc
*/
/*
    main_1
        Tien trinh chinh thuc thi Voi nuoc va cac Sinh vien
        Doi den khi cac tien trinh ket thuc
*/

#include "syscall.h"
#include "fileutils.h"

bool solve(int* a, int n, int fOutput);

int main() {
    int nTest;
    int isSuccess, readStatus;
    int n, i;
    int a[10];
    int fInput, fOutput;
    int pipeId;

    // semaphore quan ly khi co yeu cau uong nuoc
    if ((isSuccess = CreateSemaphore("open_water", 0)) == -1)
        Exit(1);   // loi khong the tao semaphore
    // semaphore quan ly 2 voi nuoc
    if ((isSuccess = CreateSemaphore("faucet", 2)) == -1)
        Exit(1);
    // semaphore quan ly doc xuat file student.txt doc quyen
    if ((isSuccess = CreateSemaphore("file_mutex", 1)) == -1)
        Exit(1);
    // semaphore quan ly tien trinh Voi nuoc 
    //  khi co yeu cau (lay hoac tra) voi nuoc
    if ((isSuccess = CreateSemaphore("request_water", 0)) == -1)
        Exit(1);
    // semaphore quan ly truy xuat tai nguyen chung voi nuoc doc quyen
    if ((isSuccess = CreateSemaphore("pipe_mutex", 1)) == -1)
        Exit(1);
    // semaphore quan ly truy xuat file output
    if ((isSuccess = CreateSemaphore("output_mutex", 1)) == -1)
        Exit(1);
    
    // cho voi nuoc chay
    pipeId = Exec("pipe");
    if (pipeId == -1) {
        PrintString("\nCan't execute process pipe\n");
        Exit(1);
    }
    // doc input
    fInput = Open("input.txt", 1);
    if (fInput == -1) { 
        PrintString("\nOpen file failed!\n");
        Exit(1);
    }
    // tao file output.txt de ghi ket qua
    if (CreateFile("output.txt") == -1) {
        PrintString("\nCreate file output.txt failed!\n");
        Exit(1);
    }
    fOutput = Open("output.txt", 0);


    readStatus = ReadInt(fInput, &nTest);
    while (nTest--) {
        n = 0;
        // doc mang cac sinh vien
        while ((readStatus = ReadInt(fInput, &a[n++])) == 1);
        // giai bai toan dong bo
        if (!solve(a, n, fOutput)) {
            // co loi phat sinh
            Close(fInput);
            Close(fOutput);
            Exit(1);
        }
    }
    Close(fInput);
    Close(fOutput);
    Exit(0);
}

bool solve(int* a, int n, int fOutput) {
    int answer[10]; // voi nuoc cho tung sinh vien
    int studentPid[10]; // luu tru id cua process student tuong ung
    int fStudent;
    int isSuccess;
    int i, studentId, faucetId;
    int fResult;
    CreateFile("result.txt"); // luu ket qua tam thoi
    // tao file luu index va so lit nuoc cua sinh vien 
    for (i = 0; i < n; ++i) {
        Wait("file_mutex"); // cho phep 1 student ghi va doc de lay du lieu
        isSuccess = CreateFile("student.txt");
        if (isSuccess == -1) {
            Signal("file_mutex");
            PrintString("\nCan't create file student.txt\n");
            return false;
        }
        fStudent = Open("student.txt", 0);
        // viet id va so lit nuoc cua sinh vien thu i
        WriteNum(fStudent, i);
        WriteChar(fStudent, ' ');
        WriteNum(fStudent, a[i]);
        Close(fStudent);
        // tien trinh sinh vien moi
        studentPid[i] = Exec("student_1");
        if (studentPid[i] == -1) {
            Signal("file_mutex");
            PrintString("\n Can't excecute process student\n");
            return false;
        }
    }
    // join cac process student
    for (i = 0; i < n; ++i)
        Join(studentPid[i]);
    // doc ket qua tu result.txt va doi ve format output.txt
    // vi cac tien trinh gianh duoc voi nuoc khong theo thu tu input
    // nen ta can anh xa ve dung thu tu
    // file request.txt luu N dong: studentId faucetId
    fResult = Open("result.txt", 1);

    for (i = 0; i < n; ++i) {
        ReadInt(fResult, &studentId);
        ReadInt(fResult, &faucetId);
        answer[studentId] = faucetId;
    }
    Close(fResult);
    // ghi ket qua cua thoi diem
    for (i = 0; i < n; ++i) {
        WriteNum(fOutput, answer[i]);
        WriteChar(fOutput, ' ');
    }
    WriteChar(fOutput, '\n');

    return true;
}