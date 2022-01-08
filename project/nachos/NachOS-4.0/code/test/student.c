#include "syscall.h"

void WriteInt(int fileId, int value);

int main() {
    int i, delayTime;
    for (i = 0; i < 10; ++i) {
        Wait("faucet");
        PrintNum(GetPID());
        PrintChar(' ');
        delayTime = RandomNum() % 10000;
        while (delayTime--);
        Signal("faucet");
    }
    Exit(0);
}

void WriteInt(int fileId, int value) {
    char digits[3];
    int k, i;
    k = 0;
    while (value > 0) {
        digits[k++] = value % 10 + '0';
        value /= 10;
    }
    for (i = 0; i < k; ++i)
        Write(&digits[i], 1, fileId);
}