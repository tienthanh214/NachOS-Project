#include "syscall.h"

// Cau h: In bang ma ASCII
int main() {
    int i = 5;

    // Ten bang ma ASCII
    PrintString("Bang ma ASCII:");
    PrintChar('\n');

    // In cac ten cot trong bang
    PrintChar('\t');
    PrintString("Dec");
    PrintChar('\t');
    PrintString("Char");

    PrintChar('\t');
    PrintChar('|');
    PrintChar('\t');
    PrintString("Dec");
    PrintChar('\t');
    PrintString("Char");

    PrintChar('\t');
    PrintChar('|');
    PrintChar('\t');
    PrintString("Dec");
    PrintChar('\t');
    PrintString("Char");
    PrintChar('\n');

    // In duong vien cua bang
    PrintString("------------------------");
    PrintString("------------------------");
    PrintString("------------------------");
    PrintChar('\n');

    // In bang ASCII thanh 3 cot
    
    for (i = 32; i < 64; ++i)     // Cac ki tu doc duoc tu 32 toi 126
    {
        PrintChar('\t');

        // Cot 1
        PrintNum(i);
        PrintChar('\t');
        PrintChar(i);

        PrintChar('\t');
        PrintChar('|');
        PrintChar('\t');

        // Cot 2
        PrintNum(i + 32);
        PrintChar('\t');
        PrintChar(i + 32);

        PrintChar('\t');
        PrintChar('|');
        PrintChar('\t');

        if (i + 64 != 127)    // Neu ki tu gia tri 127 thi khong in
        {
            // Cot 3
            PrintNum(i + 64);
            PrintChar('\t');
            PrintChar(i + 64);
        }
        PrintChar('\n');
    }

  Halt();
  /* not reached */
}


