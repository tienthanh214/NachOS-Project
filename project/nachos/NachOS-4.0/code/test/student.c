/*
    Chuong trinh sinh vien tranh chap voi nuoc
*/

#include "syscall.h"

void WriteInt(int fileId, int value);
void WriteChar(int fileId, char c);

int main()
{
    int i, delayTime;
    int fOutput, lenOutput;
    // lay 10 lit nuoc
    for (i = 0; i < 10; ++i)
    {
        // chi cho 1 sinh vien lay nuoc
        Wait("faucet");
        // ghi ket qua vao cuoi file output.txt
        fOutput = Open("output.txt", 0);
        lenOutput = Seek(-1, fOutput);
        Seek(lenOutput, fOutput);

        WriteInt(fOutput, GetPID());
        WriteChar(fOutput, ' ');
        Close(fOutput);
        // gia lap qua trinh lay nuoc
        delayTime = RandomNum() % 10000;
        while (delayTime--);
        // lay nuoc xong, tra voi nuoc
        Signal("faucet");
    }
    // ket thuc tien trinh
    Exit(0);
}
// in mot so nguyen value ra file fileId
void WriteInt(int fileId, int value)
{
    char digits[3];
    int k, i;
    k = 0;
    while (value > 0)
    {
        digits[k++] = value % 10 + '0';
        value /= 10;
    }
    for (i = 0; i < k; ++i)
        Write(&digits[i], 1, fileId);
}
// in mot ki tu c ra file file
void WriteChar(int fileId, char c)
{
    Write(&c, 1, fileId);
}
