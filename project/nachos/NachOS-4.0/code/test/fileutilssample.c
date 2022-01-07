#include "syscall.h"
#include "fileutils.h"

int main()
{
    int id, i, cnt, err;
    err = 0;
    // CreateFile("test.txt");
    // 1 la binh thuong
    // -2 la het file
    // -3 la het dong

    id = Open("test.txt", 0);
    while (err != END_OF_FILE)
    {
        err = ReadInt(id, &cnt);
        if (err == END_OF_LINE)
        {
            PrintNum(cnt);
            PrintChar('\n');
        }
        else if (err == HEAD_OF_LINE)
        {
            PrintNum(cnt);
            PrintChar(' ');
        }
        else
        {
            /* Don't print when End Of File */
        }
    }

    Close(id);

    Halt();
    /* not reached */
}
