#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; ++i) {
        Wait("ping");
        PrintChar('A');
        Signal("pong");
    }
    Exit(0);
}