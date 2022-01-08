#include "syscall.h"

int main() {
    while (1) {
        Wait("open_water");
        Wait("faucet");
        // TODO: read id - value from "student.txt"

        // TODO: run loop simulate drink time

        // TODO: write id - value into file "result.txt"
        Signal("faucet");
    }
    Exit(0);
}