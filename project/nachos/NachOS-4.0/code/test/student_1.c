/* student.c
    Sinh vien yeu cau uong nuoc
*/

#include "syscall.h"
#include "fileutils.h"

int main() {
    int fStudent, fFaucet;
    int value;
    int studentId;
    int faucetId, fRequest, fResult;
    int readStatus, i;
    // ----- doc id va so lit nuoc cua sinh vien nay` -----
    fStudent = Open("student.txt", 1);
    if (fStudent == -1) { // khong the mo file
        Signal("file_mutex");
        PrintString("\n Can't open file student.txt");
        Exit(1);
    }
    ReadInt(fStudent, &studentId);  
    ReadInt(fStudent, &value);
    Close(fStudent);
    Signal("file_mutex"); // tiep tuc o main
    
    // ---------- lay nuoc ---------------
    Wait("faucet"); // chi toi da 2 sinh vien duoc vao
    // gui yeu cau den pipe cung cap nuoc
    Wait("pipe_mutex");
    if (CreateFile("request.txt") == -1) {
        PrintString("\nCan't create file request.txt\n");
        Exit(1);
    } 
    fRequest = Open("request.txt", 0);
    WriteNum(fRequest, 0);     // yeu cau lay nuoc
    WriteChar(fRequest, ' ');
    WriteNum(fRequest, value);
    Close(fRequest);

    Signal("request_water"); // yeu cau process pipe de lay nuoc
    Wait("open_water"); // doi den khi duoc phep uong nuoc
    // nhan ket qua tra ve tu process pipe
    fFaucet = Open("faucet.txt", 1);
    ReadInt(fFaucet, &faucetId);
    Close(fFaucet);
    Signal("pipe_mutex");   // doc xuat file hoan tat   

    // -------- ghi ket qua vao file -------------
    Wait("output_mutex");
    fResult = Open("result.txt", 0);
    Seek(Seek(-1, fResult), fResult);
    WriteNum(fResult, studentId);
    WriteChar(fResult, ' ');
    WriteNum(fResult, faucetId);
    WriteChar(fResult, '\n');
    Close(fResult);
    Signal("output_mutex");

    // ------ gia lap sinh vien lay nuoc ----------
    for (i = 0; i < value * 1000; ++i);

    // ------ tra lai voi nuoc  ---------
    Wait("pipe_mutex");
    // if (CreateFile("request.txt") == -1) {
    //     PrintString("\nCan't remove data in file request.txt\n");
    //     Exit(1);
    // } 
    
    // gui yeu cau tra lai voi nuoc
    fRequest = Open("request.txt", 0);

    WriteNum(fRequest, 1);     // yeu cau tra voi nuoc
    WriteChar(fRequest, ' ');
    WriteNum(fRequest, faucetId - 1);
    Close(fRequest);
    // 
    Signal("request_water"); // gui request tra lai nuoc cho pipe
    Wait("open_water"); // cho den khi da tra xong
    Signal("pipe_mutex");   // doc xuat file hoan tat    
    // lay nuoc hoan thanh, tra lai voi nuoc
    Exit(0);
}