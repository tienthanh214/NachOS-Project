// Help Program introduce group and describe sort program and ascii program
#include"syscall.h"

int main(){
    // in thong tin nhom
    PrintString("
=================================================================
        Group Member:
19120036 - Nguyễn Đăng Tiến Thành
19120176 - Lê Công Bình
19120200 - Nguyễn Tam Dũng
=================================================================");
// gioi thieu ve chuong trinh ascii va sort
    PrintString("\n
command '-x ascii' : ascii program print the ascii character table.
command '-x sort'  : sort program sort an integer array by using bubble sort.\n
");
    Halt();
}