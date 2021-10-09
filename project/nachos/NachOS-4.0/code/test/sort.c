// /* sort.c 
//  *    Test program to sort a large number of integers.
//  *
//  *    Intention is to stress virtual memory system.
//  *
//  *    Ideally, we could read the unsorted array off of the file system,
//  *	and store the result back to the file system!
//  */


// /*
// #define UNIX
// #define UNIX_DEBUG
// */

// #ifdef UNIX
// #include <stdio.h>
// #define Exit exit
// #else
// #include "syscall.h"
// #endif /* UNIX */

// #define SIZE (1024)

// int A[SIZE];	/* size of physical memory; with code, we'll run out of space!*/

// int
// main()
// {
//     int i, j, tmp;

//     /* first initialize the array, in reverse sorted order */
//     for (i = 0; i < SIZE; i++) {
//         A[i] = (SIZE-1) - i;
//     }

//     /* then sort! */
//     for (i = 0; i < SIZE; i++) {
//         for (j = 0; j < (SIZE-1); j++) {
// 	   if (A[j] > A[j + 1]) {	/* out of order -> need to swap ! */
// 	      tmp = A[j];
// 	      A[j] = A[j + 1];
// 	      A[j + 1] = tmp;
//     	   }
//         }
//     }

// #ifdef UNIX_DEBUG
//     for (i=0; i<SIZE; i++) {
//         printf("%4d ", A[i]);
// 	if (((i+1) % 15) == 0) {
// 		printf("\n");
//         }
//         if (A[i] != i) {
//             fprintf(stderr, "Out of order A[%d] = %d\n", i, A[i]);
//             Exit(1);
//         }   
//     }
//     printf("\n");
// #endif /* UNIX_DEBUG */

//     for (i=0; i<SIZE; i++) {
//         if (A[i] != i) {
//             Exit(1);
//         }   
//     }

//     Exit(0);
// }

#include "syscall.h"

int n;
int A[100];
bool isAsc;

void readInput() {
    int i;
    PrintString("Enter number of elements n = ");
    n = ReadNum();
    PrintString("Enter array = ");
    for (i = 0; i < n; ++i)
        A[i] = ReadNum();
    
    PrintString("Ascending/Descending enter [0/1]: ")
    isAsc = ReadChar() == '0';
}

bool greater(int x, int y) {
    return x > y;
}

bool less(int x, int y) {
    return x < y;
}

/*  sort integer array using Bubble sort algorithm
        order by function pointer compare
*/
void bubbleSort(bool (*compare)(int, int)) {
    int i, j, temp;
    for (i = 0; i < n; ++i) {
        for (j = n - 1; j > i; --j) 
            if (compare(A[j - 1], A[j])) {
                temp = A[j - 1];
                A[j - 1] = A[j];
                A[j] = temp;
            }
    }
}

int main() {
    readInput();
    bubbleSort(isAsc ? greater : less);
    Halt();
}