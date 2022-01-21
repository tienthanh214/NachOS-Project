// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -n -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you are using the "stub" file system, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "addrspace.h"
#include "machine.h"
#include "noff.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
#ifdef RDATA
    noffH->readonlyData.size = WordToHost(noffH->readonlyData.size);
    noffH->readonlyData.virtualAddr = 
           WordToHost(noffH->readonlyData.virtualAddr);
    noffH->readonlyData.inFileAddr = 
           WordToHost(noffH->readonlyData.inFileAddr);
#endif 
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);

#ifdef RDATA
    DEBUG(dbgAddr, "code = " << noffH->code.size <<  
                   " readonly = " << noffH->readonlyData.size <<
                   " init = " << noffH->initData.size <<
                   " uninit = " << noffH->uninitData.size << "\n");
#endif
}

/*
    19CNTN - HCMUS
    19120036, 19120176, 19120200
*/

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//----------------------------------------------------------------------

/*
    Now support multiprogramming
*/
AddrSpace::AddrSpace()
{
    numPages = 0;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    unsigned int i;
    for (i = 0; i < numPages; ++i)
        kernel->gPhysPageBitMap->Clear(pageTable[i].physicalPage);
    delete pageTable;
}


//----------------------------------------------------------------------
// AddrSpace::Load
// 	Load a user program into memory from a file.
//
//	Assumes that the page table has been initialized, and that
//	the object code file is in NOFF format.
//
//	"fileName" is the file containing the object code to load into memory
//----------------------------------------------------------------------

/*
    modified this, now can multiprogramming
*/

bool 
AddrSpace::Load(char *fileName) 
{
    OpenFile *executable = kernel->fileSystem->Open(fileName);
    NoffHeader noffH;
    unsigned int size;

    if (executable == NULL) {
        cerr << "Unable to open file " << fileName << "\n";
        return FALSE;
    }

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // allow only one process
    kernel->addrLock->P();

#ifdef RDATA
// how big is address space?
    size = noffH.code.size + noffH.readonlyData.size + noffH.initData.size +
           noffH.uninitData.size + UserStackSize;	
                                                // we need to increase the size
						// to leave room for the stack
    // printf("Load Addrspace: %d %d %d %d %d %d\n", noffH.code.virtualAddr, noffH.code.size, noffH.initData.virtualAddr, noffH.initData.size, noffH.readonlyData.virtualAddr, noffH.readonlyData.size);
#else
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
#endif
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    
    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory 

    if ((int)numPages > kernel->gPhysPageBitMap->NumClear()) {
        printf("\nAddrSpace::Load: not enough memory for new process");
        numPages = 0;
        delete executable;
        kernel->addrLock->V();
        return FALSE;
    }

    // cap phat frame cho user program
    pageTable = new TranslationEntry[numPages];
    for (unsigned int i = 0; i < numPages; ++i) {
        pageTable[i].virtualPage = i;	
        pageTable[i].physicalPage = kernel->gPhysPageBitMap->FindAndSet();  // tim trang trong
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  
        bzero(&kernel->machine->mainMemory[pageTable[i].physicalPage * PageSize], PageSize);
    }
    // allocate done
    kernel->addrLock->V();

    DEBUG(dbgAddr, "Initializing address space: " << numPages << ", " << size);

// then, copy in the code and data segments into memory
// read user program into allocated frame
    
    // setup position read into memory
    unsigned int i = 0;
    unsigned int numCodePage = divRoundUp(noffH.code.size, PageSize);
    int lastCodePageSize = (noffH.code.size - 1) % PageSize + 1;
    int firstDataPageSize = min(PageSize - lastCodePageSize, noffH.initData.size);
    int lastDataPageSize = (noffH.initData.size - firstDataPageSize - 1) % PageSize + 1;
    unsigned int numDataPage = divRoundUp(noffH.initData.size - firstDataPageSize, PageSize);
    
    // copy code into memory
    if (noffH.code.size > 0) {
        DEBUG(dbgAddr, "Initializing code segment.");
	    DEBUG(dbgAddr, noffH.code.virtualAddr << ", " << noffH.code.size);
        // copy code data into memory
        for (i = 0; i < numCodePage; ++i) {
            executable->ReadAt(
                &(kernel->machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage * PageSize,
                (i < numCodePage - 1) ? PageSize : lastCodePageSize,
                noffH.code.inFileAddr + i * PageSize
            );
        }
    }

    // copy initdata into memory
    if (noffH.initData.size > 0) {
        DEBUG(dbgAddr, "Initializing data segment.");
	    DEBUG(dbgAddr, noffH.initData.virtualAddr << ", " << noffH.initData.size);
        // read first segment of Data into remain of last page code
        if (lastCodePageSize != PageSize && numCodePage > 0) { // if remain slot
            executable->ReadAt(
                &(kernel->machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i - 1].physicalPage * PageSize + lastCodePageSize,
                firstDataPageSize,
                noffH.initData.inFileAddr
            );
        }
        // copy init data into memory
        for (unsigned int j = 0; j < numDataPage; ++j, ++i) {
            executable->ReadAt(
                &(kernel->machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage * PageSize,
                (j < numDataPage - 1) ? PageSize : lastDataPageSize,
                noffH.initData.inFileAddr + firstDataPageSize + j * PageSize
            );
        }
    }

#ifdef RDATA
    // copy readonly to main
    if (noffH.readonlyData.size > 0) {
        DEBUG(dbgAddr, "Initializing read only data segment.");
	    DEBUG(dbgAddr, noffH.readonlyData.virtualAddr << ", " << noffH.readonlyData.size);
        int offset = (noffH.code.size + noffH.initData.size - 1) % PageSize + 1;
        int firstReadDataSize = min(
            PageSize - offset,
            noffH.readonlyData.size);
        int lastReadDataSize = (noffH.readonlyData.size - firstReadDataSize - 1) % PageSize + 1;
        unsigned int numReadPage = divRoundUp(noffH.readonlyData.size - firstReadDataSize, PageSize);
        if (offset != PageSize && i > 0) {
            executable->ReadAt(
                &(kernel->machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i - 1].physicalPage * PageSize + offset,
                firstReadDataSize,
                noffH.readonlyData.inFileAddr
            );
        }

        for (unsigned int j = 0; j < numReadPage; ++j, ++i) {
            executable->ReadAt(
                &(kernel->machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage * PageSize,
                (j < numReadPage - 1) ? PageSize : lastReadDataSize,
                noffH.readonlyData.inFileAddr + firstReadDataSize + j * PageSize
            );
        }

        // executable->ReadAt(
		// &(kernel->machine->mainMemory[noffH.readonlyData.virtualAddr]),
		// 	noffH.readonlyData.size, noffH.readonlyData.inFileAddr);
    }
#endif

    delete executable;			// close file
    return TRUE;			// success
}

//----------------------------------------------------------------------
// AddrSpace::Execute
// 	Run a user program using the current thread
//
//      The program is assumed to have already been loaded into
//      the address space
//
//----------------------------------------------------------------------

void 
AddrSpace::Execute() 
{

    kernel->currentThread->space = this;

    this->InitRegisters();		// set the initial register values
    this->RestoreState();		// load page table register

    kernel->machine->Run();		// jump to the user progam

    ASSERTNOTREACHED();			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}


//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    Machine *machine = kernel->machine;
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start", which
    //  is assumed to be virtual address zero
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    // Since instructions occupy four bytes each, the next instruction
    // after start will be at virtual address four.
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG(dbgAddr, "Initializing stack pointer: " << numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, don't need to save anything!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    kernel->machine->pageTable = pageTable;
    kernel->machine->pageTableSize = numPages;
}


//----------------------------------------------------------------------
// AddrSpace::Translate
//  Translate the virtual address in _vaddr_ to a physical address
//  and store the physical address in _paddr_.
//  The flag _isReadWrite_ is false (0) for read-only access; true (1)
//  for read-write access.
//  Return any exceptions caused by the address translation.
//----------------------------------------------------------------------
ExceptionType
AddrSpace::Translate(unsigned int vaddr, unsigned int *paddr, int isReadWrite)
{
    TranslationEntry *pte;
    int               pfn;
    unsigned int      vpn    = vaddr / PageSize;
    unsigned int      offset = vaddr % PageSize;

    if(vpn >= numPages) {
        return AddressErrorException;
    }

    pte = &pageTable[vpn];

    if(isReadWrite && pte->readOnly) {
        return ReadOnlyException;
    }

    pfn = pte->physicalPage;

    // if the pageFrame is too big, there is something really wrong!
    // An invalid translation was loaded into the page table or TLB.
    if (pfn >= NumPhysPages) {
        DEBUG(dbgAddr, "Illegal physical page " << pfn);
        return BusErrorException;
    }

    pte->use = TRUE;          // set the use, dirty bits

    if(isReadWrite)
        pte->dirty = TRUE;

    *paddr = pfn*PageSize + offset;

    ASSERT((*paddr < MemorySize));

    //cerr << " -- AddrSpace::Translate(): vaddr: " << vaddr <<
    //  ", paddr: " << *paddr << "\n";

    return NoException;
}
