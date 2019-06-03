//*****************************************************************************/
//$COMMON.H$
//   File Name:  Allocator.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Allocator
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      04    05/12/98  Lafreniere          Morph
//      Description: SCR-311. Eliminate the extra 4 bytes per instance imposed
//          by this class. 
//
//      03    03/30/98  Lafreniere          Morph
//      Description: SCR-277. Added a return of E_Bool to the operator++ 
//          function to eliminate a compiler warning message. 
//
//      02    01/11/98  Lafreniere          Morph
//      Description: SCR-137. Cleaned up the Allocator class design. The 
//          dynamic mode now only takes 4 bytes or overhead per instance like
//          static mode. 
//
//      01    06/13/97  Lafreniere          Morph
//      Description: Initial Revision
//
//*****************************************************************************/
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <string.h>
#include "CommonTypes.h"

#define ALLOCATOR_H_INTERFACE_OVERRIDE(class, instances, override) \
    private: \
        static Allocator* S_Allocator; \
        void operator delete(void*) {} \
    public: \
        void* operator new(size_t size) { \
            if (!S_Allocator) \
                S_Allocator = \
                   ::new Allocator(#class, instances, sizeof(class), override);\
            void* pBlock = S_Allocator->Allocate(); \
            ASSERTION(pBlock); \
            return pBlock; \
        } \
        virtual void Delete(void) { \
            this->class::~class(); \
            S_Allocator->Deallocate(this); \
        } \
        static USHORT S_PeakInstancesIs(void) { \
            return S_Allocator ? S_Allocator->PeakInstancesIs() : 0; \
        } \
        static size_t S_BlockSizeIs(void) { \
            return S_Allocator ? S_Allocator->BlockSizeIs() : 0; \
        } \
    private:

// A macro that defines the .h interface in the client's class
#define ALLOCATOR_H_INTERFACE(class, instances) \
    ALLOCATOR_H_INTERFACE_OVERRIDE(class, instances, 0)

// A macro that defines the .cpp interface in the client's class
#define ALLOCATOR_CPP_INTERFACE(class) \
    Allocator* class::S_Allocator = NULL;

class Allocator 
{
public:
    Allocator
        (
            const CHAR* name,
            USHORT instances,
            size_t instanceSize, 
            size_t overrideSize = 0
        );
    ~Allocator() { ASSERTION(0); }

    //$COMMON.ATTRIBUTE$
    //  Name: Info
    //  Description: To hold pertinent information for an Allocator.
    //  Units: 
    //  Range: 
    struct Info 
    {
        const CHAR* name;
        USHORT instances;
        USHORT peakInstances;
        size_t blockSize;
        size_t instanceSize;
        size_t overrideSize;
    };

    void* Allocate();
    void Deallocate(void*);
    Info GetInfo(void);
    size_t BlockSizeIs(void) { return BlockSize; }
    USHORT PeakInstancesIs(void) { return 0; }

    friend class AllocatorIterator;

private:
    // prevent copying
    Allocator(const Allocator&);
    void operator=(Allocator&);

    void Insert(void*);
    void* Remove();

    struct Block {
        Block* Next;
    };
    
    //$COMMON.ATTRIBUTE$
    //  Name: Head
    //  Description: The address to the head of the block list. 
    //  Units: 
    //  Range: 
    Block* Head;
    
    //$COMMON.ATTRIBUTE$
    //  Name: PeakInstances
    //  Description: The maximum number of outstanding objects reached during
    //      execution. 
    //  Units: 
    //  Range: 
    USHORT PeakInstances;

    //$COMMON.ATTRIBUTE$
    //  Name: NAME
    //  Description: The name of the Allocator instance.
    //  Units: 
    //  Range: 
    const CHAR* const NAME;
    
    //$COMMON.ATTRIBUTE$
    //  Name: INSTANCE_SIZE
    //  Description: The size of a class instance. This will be used as the 
    //      default block size.
    //  Units: 
    //  Range: 
    const size_t INSTANCE_SIZE;
    
    //$COMMON.ATTRIBUTE$
    //  Name: OVERRIDE_SIZE
    //  Description: The user supplied size of a block that must be greater
    //      than the BLOCK_SIZE.
    //  Units: 
    //  Range: 
    const size_t OVERRIDE_SIZE;

    //$COMMON.ATTRIBUTE$
    //  Name: BlockSize
    //  Description: The size of each memory pool block.
    //  Units: 
    //  Range: 
    size_t BlockSize;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Head
    //  Description: A pointer to the first Allocator in the linked list.
    //  Units: 
    //  Range: 
    static Allocator* S_Head;
    
    //$COMMON.ATTRIBUTE$
    //  Name: S_Tail
    //  Description: A pointer to the last Allocator in the linked list.
    //  Units: 
    //  Range: 
    static Allocator* S_Tail;

    //$COMMON.ATTRIBUTE$
    //  Name: Next
    //  Description: A pointer to the next Allocator in the linked list.
    //  Units: 
    //  Range: 
    Allocator* Next;

#ifndef ALLOCATOR_DYNAMIC_MODE
    void* CalculateBlockAddr(USHORT blockIndex)
    {
        return (void *)(pPool + (blockIndex * BlockSize));
    }

    //$COMMON.ATTRIBUTE$
    //  Name: INSTANCES
    //  Description: The maximum allowed number of instances. 
    //  Units: 
    //  Range: 
    USHORT INSTANCES;

    //$COMMON.ATTRIBUTE$
    //  Name: pPool
    //  Description: The starting address of the memory pool.
    //  Units: 
    //  Range: 
    const CHAR* pPool;
    
    //$COMMON.ATTRIBUTE$
    //  Name: pPoolEnd
    //  Description: A ending address of the memory pool.
    //  Units: 
    //  Range: 
    const CHAR* pPoolEnd;
#endif

// methods for enable and disable of interrupts
#if defined(_WINDOWS) || defined(TEST_MODE)
	void DisableInt() { }
	void EnableInt() { }
#else
	void DisableInt() { asm (" di"); }
	void EnableInt() { asm (" ei"); }
#endif
};

class AllocatorIterator
{
public:
    AllocatorIterator() { Reset(); }
    ~AllocatorIterator() {}

    // Increment the iterator to the next Allocator. Returns FALSE when
    // end of list has been found.       
    E_Bool operator++(int) 
    { 
        if (Current->Next)
        {
            Current = Current->Next;
            return eTrue;
        }
        return eFalse;
    }
    // Finds the Allocator which matches the name. If no match is found, 
    // AllocatorIterator just points to the head.
    void operator=(const CHAR* name) 
    {
        Reset();

        if (!name) return;

        while (Current)
        {
            if (!strcmp(Current->NAME, name))
                break;
            Current = Current->Next;
        }
    }
    Allocator* operator->() const { return Current; }
private:
    // prevent copying
    AllocatorIterator(const AllocatorIterator&);
    void operator=(AllocatorIterator&);

    void Reset(void) 
    { 
        Current = Allocator::S_Head;
        DEBUG_ASSERTION(Current);
    }

    //$COMMON.ATTRIBUTE$
    //  Name: Current
    //  Description: Allows user to move though all instances of Allocator.
    //  Units: 
    //  Range: 
    Allocator* Current;
};

#endif //ALLOCATOR_H



