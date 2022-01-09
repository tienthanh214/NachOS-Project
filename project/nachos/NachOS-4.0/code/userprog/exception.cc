// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define MaxFileLength 32
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

/* Increase program counter */
void IncreasePC()
{
    /* set previous program counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set program counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next program counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

/*  
    Copy buffer from User space to Kernel memory space 
    Input:  User space address, length of buffer
    Output: Buffer 
*/
char *User2System(int virtualAddr, int limit)
{
    int idx;
    int oneChar;
    char *kernelBuffer = NULL;
    kernelBuffer = new char[limit + 1];
    if (kernelBuffer == NULL)
        return kernelBuffer;
    // printf("Copying User to Kernel");
    for (idx = 0; idx < limit; ++idx)
    {
        kernel->machine->ReadMem(virtualAddr + idx, 1, &oneChar);
        kernelBuffer[idx] = oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuffer;
}

/*  
    Copy buffer from Kernel space to User memory space  
    Input:  - address in user space
            - limit of buffer
            - buffer kernel memory
    Output: number of bytes copied
*/
int System2User(int virtualAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return 0;
    int i = 0, oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtualAddr + i, 1, oneChar);
        ++i;
    } while (i < len && oneChar != 0);
    return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which)
    {
    case NoException:
        return;

    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

            SysHalt();

            ASSERTNOTREACHED();
            break;

        case SC_Add:
        {
            DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

            /* Process SysAdd Systemcall*/
            int result;
            result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                            /* int op2 */ (int)kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Add returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);
            /* Modify return point */
            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // Xu ly system call ReadNum
        case SC_ReadNum:
        {
            int num;
            num = SysReadNum();                          // system read integer number
            kernel->machine->WriteRegister(2, (int)num); // write the return value to register 2

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // Xu ly system call PrintNum
        case SC_PrintNum:
        {
            int num = (int)kernel->machine->ReadRegister(4); // get the number to print from register 4
            SysPrintNum(num);                                // system print number

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall ReadChar
        case SC_ReadChar:
        {
            char c;
            c = SysReadChar();                    // read a character
            kernel->machine->WriteRegister(2, c); // write the return value to register 2

            IncreasePC();

            return;
            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall PrintChar
        case SC_PrintChar:
        {
            char c = kernel->machine->ReadRegister(4); // get the character to print from register 4
            SysPrintChar(c);                           // print character

            IncreasePC();

            return;
            ASSERTNOTREACHED();
            break;
        }
        case SC_RandomNum:
        {
            kernel->machine->WriteRegister(2, SysRandomNumber()); // write result to register 2

            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall ReadString
        case SC_ReadString:
        {
            int virtualAddr;
            char *buffer;
            int length;
            virtualAddr = kernel->machine->ReadRegister(4); // get buffer' address
            length = kernel->machine->ReadRegister(5);      // maximum length of input string
            buffer = User2System(virtualAddr, length);      // copy string from User space to Kernel space
            SysReadString(buffer, length);                  // system read string
            System2User(virtualAddr, length, buffer);       // return string to User space
            delete buffer;

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall PrintString
        case SC_PrintString:
        {
            int virtualAddr = kernel->machine->ReadRegister(4); // get address of buffer
            char *buffer = User2System(virtualAddr, 255);       // copy string (max 255 byte) from User space to Kernel space
            SysPrintString(buffer);                             // print string
            delete[] buffer;

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xuly syscall CreateFile tao file
        case SC_CreateFile:
        {

            // Tao ra file voi tham so la ten file
            int virtAddr;
            char *filename;
            virtAddr = kernel->machine->ReadRegister(4); //Doc dia chi cua file tu thanh ghi R4
            filename = User2System(virtAddr, MaxFileLength + 1); // return string to User space
            SysCreateFile(filename);
            delete[] filename;

            IncreasePC();
            return;
            //Tao file thanh cong
        }
        // xu ly syscall Open mo file
        case SC_Open: // Mo mot file voi type read-and-write hoac only-read
        {
            int virtAddr = kernel->machine->ReadRegister(4); // Doc tham so filename
            int type = kernel->machine->ReadRegister(5);     // Doc tham so type
            char *filename;
            filename = User2System(virtAddr, MAX_FILENAME_LENGTH + 1);
            OpenFileID result = SysOpen(filename, type); //Mo file, tra ve type neu thanh cong, tra ve -1 neu that bai
            kernel->machine->WriteRegister(2, result);
            delete[] filename;
            IncreasePC();
            return;
            break;
        }
        // xu ly syscall Close dong file
        case SC_Close:
        {
            int id = kernel->machine->ReadRegister(4);  // Lay id cua file tu thanh ghi so 4
            int result = SysClose(id);                  // Goi ham dong file
            kernel->machine->WriteRegister(2, result);  // Tra ve ket qua vao thanh ghi so 2
            IncreasePC();
            return;
            break;
        }
        // xu ly syscall Exec thuc thi mot tien trinh
        case SC_Exec:
        {
            int virtualAddr = kernel->machine->ReadRegister(4); // Doc tham so dia chi buffer
            char *name;
            name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);   // Lay ten file 
            SysExec(name);
            if (name != NULL)   // Neu ten file khac null thi giai phong bo nho
                delete[] name;
            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall Join
        case SC_Join:
        {
            int pid;
            pid = kernel->machine->ReadRegister(4);         // Doc tham so id cua tien trinh
            kernel->machine->WriteRegister(2, SysJoin(pid));// Ghi ket qua tu viec goi ham SysJoin 
            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall Exit dung tien trinh
        case SC_Exit:
        {
            int exitCode;
            exitCode = kernel->machine->ReadRegister(4);    // Doc tham so exit code
            SysExit(exitCode);
            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall GetPID lay process id
        case SC_GetPID:
        {
            kernel->machine->WriteRegister(2, kernel->currentThread->processID);
            IncreasePC();
            return;
        }
        // xu ly syscall CreateSemaphore
        case SC_CreateSemaphore:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            int semVal = kernel->machine->ReadRegister(5);
            char *name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysCreateSemaphore(name, semVal);
            if (name != NULL)
                delete[] name;
            // ghi ket qua tra ve
            kernel->machine->WriteRegister(2, result);
            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
        }
        // xu ly syscall Wait
        case SC_Wait:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            char *name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysWait(name);

            kernel->machine->WriteRegister(2, result);
            IncreasePC();
            return;
        }
        // xu ly syscall Signal
        case SC_Signal:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            char *name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysSignal(name);

            kernel->machine->WriteRegister(2, result);
            IncreasePC();
            return;
        }
        // xu ly syscall Read file
        case SC_Read:
        {
            int virtAddr = kernel->machine->ReadRegister(4);
            int size = kernel->machine->ReadRegister(5);
            int id = kernel->machine->ReadRegister(6);

            char *buffer = User2System(virtAddr, size);

            int res = SysRead(buffer, size, id);

            kernel->machine->WriteRegister(2, res);
            if (res != -1 && res != -2)
                System2User(virtAddr, res, buffer);

            delete buffer;
            IncreasePC();
            return;
        }
        // xu ly syscall Write file
        case SC_Write:
        {
            int virtAddr = kernel->machine->ReadRegister(4);
            int size = kernel->machine->ReadRegister(5);
            int id = kernel->machine->ReadRegister(6);
            // Lay du lieu
            char *buffer = User2System(virtAddr, size);
            // Ghi vao file
            int res = SysWrite(buffer, size, id);
            // Ghi ket qua tra ve
            kernel->machine->WriteRegister(2, res);

            delete buffer;
            IncreasePC();
            return;
        }
        // xu ly syscall Seek
        case SC_Seek:
        {
            int position = kernel->machine->ReadRegister(4);
            int id = kernel->machine->ReadRegister(5);
            // Tim den vi tri va nhan ket qua tra ve
            int res = SysSeek(position, id);
            // Ghi ket qua tra ve
            kernel->machine->WriteRegister(2, res);

            IncreasePC();
            return;
        }
        // Cac system call chua duoc xu li thi se in ra thong bao loi
        case SC_Create:
        case SC_ThreadFork:
        case SC_ThreadYield:
        case SC_ExecV:
        case SC_ThreadExit:
        case SC_ThreadJoin:
        case SC_Remove:
            cerr << "Not yet implemented system call " << type << "\n";
            SysHalt();
            break;

        default:
            cerr << "Unexpected system call " << type << "\n";
            break;
        }
        break;

    // Nhung exception khac thi in ra mot thong bao loi
    case PageFaultException:
        DEBUG(dbgSys, "No valid translation found\n");
        printf("No valid translation found\n");
        SysHalt();
        break;

    case ReadOnlyException:
        DEBUG(dbgSys, "Write attempted to page marked \"read-only\"\n");
        printf("Write attempted to page marked \"read-only\"\n");
        SysHalt();
        break;

    case BusErrorException:
        DEBUG(dbgSys, "Translation resulted in an invalid physical address\n");
        printf("Translation resulted in an invalid physical address\n");
        SysHalt();
        break;

    case AddressErrorException:
        DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space\n");
        printf("Unaligned reference or one that was beyond the end of the address space\n");
        SysHalt();
        break;

    case OverflowException:
        DEBUG(dbgSys, "Integer overflow in add or sub\n");
        printf("Integer overflow in add or sub\n");
        SysHalt();
        break;

    case IllegalInstrException:
        DEBUG(dbgSys, "Unimplemented or reserved instr\n");
        printf("Unimplemented or reserved instr\n");
        SysHalt();
        break;

    case NumExceptionTypes:
        DEBUG(dbgSys, "Number exception types\n");
        printf("Number Exception types\n");
        SysHalt();
        break;

    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }
    ASSERTNOTREACHED();
}