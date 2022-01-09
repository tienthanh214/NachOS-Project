#include "syscall.h"

void WriteInt(int fileId, int value);
void WriteChar(int fileId, char c);

int main()
{
    int i, delayTime;
    int fOutput, lenOutput;

    for (i = 0; i < 10; ++i)
    {
        // chi cho 1 sinh vien lay nuoc
        Wait("faucet");
        fOutput = Open("output.txt", 0);
        // ghi ket qua vao cuoi file
        Seek(Seek(-1, fOutput), fOutput);
        WriteInt(fOutput, GetPID());
        WriteChar(fOutput, ' ');
        Close(fOutput);
        // gia lap qua trinh lay nuoc
        delayTime = RandomNum() % 10000;
        while (delayTime--)
            ;
        // lay nuoc xong
        Signal("faucet");
    }
    Exit(0);
}

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

void WriteChar(int fileId, char c)
{
    Write(&c, 1, fileId);
}
