#line 2 "allocator.cpp"         // force __FILE__ to drop path name
/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Allocator.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The Allocator class handles the creation and storage of dynamic classes.
//      Allocator is fully re-entrant and protected for use in a multitasking 
//      system. Using this class prevents heap fragmentation and improves 
//      new/delete speed.
//
// Start of mode specific information:
//
//      There is two modes of operation for Allocator: Static mode requires that
//      the designer specify up front the expected instances of a particular
//      class. Dynamic mode, however, will continually create new object 
//      instances off the heap as required. 
//
//      The mode is selected at compile time via the define 
//      ALLOCATOR_DYNAMIC_MODE. When defined, the dynamic mode is used,
//      otherwise static mode is used. 
//
//      The dynamic mode offers the advantage of not having to specify up front
//      a preset number of instances created for a class. Static mode has the
//      advantage of consistant execution allocation/deallocation times. Also
//      static mode can make better use of storage. The Nucleus O/S adds 
//      memory to each allocation for a tracking header. Since the static mode
//      uses a single pool per class that header overhead is minimized. Dynamic 
//      mode can be used for testing so the designer does not have to 
//      worry about the number of objects. Once the design has settled, the 
//      peak number of instances for each class is set for static mode usage.
//      
//      The basic philosophy of both Allocator memory management schemes
//      is to allocate memory off the heap but never release it back to the 
//      system. When the memory is deleted, the memory block for a single object
//      is freed for use again by another object of the same type, but is not 
//      released back to the heap.
//
//      The dynamic and static modes differ in their approach to solving the 
//      problem. The static mode pre-allocates enough RAM space to handle the
//      total expected instances specified by the designer. The dynamic mode
//      creates memory chunks only as required. Therefore, the static memory
//      scheme uses a single contiguous chunk of RAM and once depleted the 
//      software faults. Dynamic mode memory chunks are not contiguous and 
//      is only limited by the heap size.
//
//      The block's linked list holds blocks that have been "deleted" to be 
//      reused by another instance of the same type. When a request is made, the 
//      list is checked first. If no free blocks exist, then a new block is 
//      obtained from the heap in dynamic mode. In static mode, another 
//      block is obtained from the memory pool. If the pool runs out, the
//      software faults. 
//
// End of mode specific information:
//
//      Each block of memory is tracked using a singly linked list. The pointer
//      is stored in the unused object space. Once a class is deleted, the 
//      memory is no longer being utilized. We need to keep the deleted 
//      memory blocks around, so we put the list pointer in that currently 
//      unused object space. When a new class is created, the pointer will 
//      be overwritten by the newly formed object. This means that every
//      object must at least be large enough to hold the 4 byte list pointer.
//
//      When a request is made using Construct(), a pointer to a block of 
//      memory is returned. When Destroy() is called, the block is "freed" to 
//      be used by another instance, however the memory is not actually 
//      released to the heap.
//      
//      To make the class easier to use, macros are used to automate the 
//      process of providing the interface from the client class to 
//      Allocator. The ALLOCATOR_H_INTERFACE macro is placed within the client
//      class definition. The ALLOCATOR_CPP_INTERFACE is placed in the .cpp
//      file, as shown in the example below:
//
//          class MyClass
//          {
//              ALLOCATOR_H_INTERFACE(MyClass, 3)
//              // remaining class definition 
//          };
//          // this line goes in .cpp file
//          ALLOCATOR_CPP_INTERFACE(MyClass)
//
//      MyClass is the class we want to construct/destroy and 3 is the total 
//      maximum expected instances of MyClass at any moment in time. 
//
//      For the user of this class to new/delete an object, no special syntax is 
//      required for the new. However, the delete requires calling the Delete()
//      method provided by the macro. Never call the operator delete when the 
//      Allocator is used, use Delete().
//      
//          MyClass* myClass = new MyClass;
//          myClass->Delete();
//
//      Statistical information can be obtained about all instances of 
//      Allocator by using the AllocatorIterator class. It allows the caller to
//      cycle through all instances of Allocator and obtain pertinent 
//      information regarding each instance. Assigning a 0 resets the iterator 
//      to the starting of the list.
//
//          AllocatorIterator allocator;
//          Allocator::Info info;
//          do {
//              info = allocator->GetInfo();
//          } while (allocator++);
//
//          allocator = 0;                // resets iterator to start of list
//
//      To find information on a particular instance, assign the allocator 
//      the name of the class. 
//      
//          allocator = "MyClass";        // sets the iterator to MyClass
//          info = allocator->GetInfo();  // returns information on MyClass
//
//      Limitations: Arrays of objects are not supported. Also cannot be used if
//      exception handling is enabled because the overloaded delete is not 
//      accessible.
//
//   Interfaces: None
//
//   Restrictions: None
//
//   Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      06    11/18/98  R. L. Stephenson    Morph
//      Description: Removed allocator from its fixed location in Flash.
//      This is no longer required to support routines in OTP. See SCR #283.
//
//      Rev:  Date:     Engineer:           Project:
//      05    07/09/98  R. L. Stephenson    Morph
//      Description: Placed allocator code in a fixed location in Flash.
//      See SCR #283.
//          
//      Rev:  Date:     Engineer:           Project:
//      04    05/12/98  Lafreniere          Morph
//      Description: SCR-311. Eliminate the extra 4 bytes per instance imposed
//          by this class. 
//
//      03    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      02    01/11/98  Lafreniere          Morph
//      Description: SCR-137. Cleaned up the Allocator class design. The 
//          dynamic mode now only takes 4 bytes or overhead per instance like
//          static mode. 
//
//      01    07/17/97  Lafreniere          Morph
//      Description: Initial Revision
//
/******************************************************************************/
#include "Allocator.h"

Allocator* Allocator::S_Head = NULL;
Allocator* Allocator::S_Tail = NULL;

//*****************************************************************************/
//    Operation Name: Allocator()
//
//    Processing: Class constructor. 
//
//    Input Parameters:
//      name - the name of the class which is using the Allocator services.
//      instanceSize - size of a single instance. 
//      overrideSize - a user defined block size. If zero, the block size 
//          is the size of instanceSize. A non-zero value overrides the 
//          instanceSize, however it must be greater than or equal to 
//          instanceSize. 
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
Allocator::Allocator
    (
        const CHAR* name,
        USHORT instances,
        size_t instanceSize, 
        size_t overrideSize
    ) : 
#ifndef ALLOCATOR_DYNAMIC_MODE
    INSTANCES(instances),
#endif
    NAME(name),
    INSTANCE_SIZE(instanceSize < sizeof(long*) ? sizeof(long*):instanceSize),
    OVERRIDE_SIZE(overrideSize)
{
    Head = NULL;
    PeakInstances = 0;

    //  IF caller supplied a block size
    //      ensure it is the correct size
    //      block size is override size
    //  ELSE
    //      block size is instance size
    if (OVERRIDE_SIZE) 
    {
        ASSERTION(OVERRIDE_SIZE >= INSTANCE_SIZE);
        BlockSize = OVERRIDE_SIZE;
    }
    else
        BlockSize = INSTANCE_SIZE;

#ifndef ALLOCATOR_DYNAMIC_MODE
    // allocate memory pool large enough to handle all expected instances
    // set a pointer to the end of the memory pool address range
    pPool = (CHAR*)new CHAR[BlockSize * INSTANCES];
    pPoolEnd = pPool + (BlockSize * INSTANCES);
#endif

    // put instance of Allocator into a linked list    
    DisableInt();
    if (!S_Head)
    {
        S_Head = this;
        S_Tail = this;
    }
    else
    {
        S_Tail->Next = this;
        S_Tail = this;
    }
    this->Next = NULL;
    EnableInt();
}

//*****************************************************************************/
//    Operation Name: Allocate()
//
//    Processing: Get a pointer to a free memory block. 
//
// Start of mode specific information:
//      If the linked list cannot offer up an existing block, then allocate a
//      new block in dynamic mode. In static mode, obtain another block from
//      the memory pool.
// End of mode specific information:
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:
//      void* - if successful returns a pointer to a memory block, otherwise
//          returns NULL.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void* Allocator::Allocate() 
{
    // if can't obtain existing block then create a new one
    void* pBlock = Remove();
    if (!pBlock)
    {
#ifdef ALLOCATOR_DYNAMIC_MODE
        pBlock = (void*)new char[BlockSize];
        PeakInstances++;
#else
        if (PeakInstances < INSTANCES)
            pBlock = CalculateBlockAddr(PeakInstances++);
#endif
    }

    return pBlock;  
}

//*****************************************************************************/
//    Operation Name: Deallocate()
//
//    Processing: Free a memory block for use by another instance.
//
//    Input Parameters: 
//      pObject* - a pointer to the object to free. 
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void Allocator::Deallocate(void* pObject)
{
#ifndef ALLOCATOR_DYNAMIC_MODE
    // ensure object address is within the memory pool address range
    ASSERTION((ULONG)pObject >= (ULONG)pPool &&
              (ULONG)pObject <= (ULONG)pPoolEnd);
#endif

    Insert(pObject);
}

//*****************************************************************************/
//    Operation Name: Insert()
//
//    Processing: Insert a memory block into the head of the list.
//
//    Input Parameters: 
//      pMemory* - a pointer to a memory block to insert into the list. 
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void Allocator::Insert(void* pMemory) 
{
    DisableInt();

    // create a pointer to the memory block
    // set the inserted block's next pointer to the existing head
    // the inserted block is the new head
    Block* pBlock = (Block*)pMemory;
    pBlock->Next = Head;
    Head = pBlock; 

    EnableInt();
}

//*****************************************************************************/
//    Operation Name: Remove()
//
//    Processing: Remove a memory block from the list head. Returns NULL if 
//      list is empty.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:
//      void* - a pointer to a memory block. 
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void* Allocator::Remove() 
{
    Block* pBlock = NULL;

    DisableInt();

    // if the list is not empty
    //      remove the head from the list
    //      the next block in line is the new head
    if (Head) {                     
        pBlock = Head;       
        Head = Head->Next;
    }
    EnableInt();

    return pBlock;
}

//*****************************************************************************/
//    Operation Name: GetInfo()
//
//    Processing: Returns an information structure about the Allocator.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:
//      Info - a structure that contains information about an instance
//          of Allocator.      
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
Allocator::Info Allocator::GetInfo(void)
{
    Info info;

    info.name = NAME;
    info.blockSize = BlockSize;
#ifdef ALLOCATOR_DYNAMIC_MODE
    info.instances = 0;     // dynamic mode not limited by preset instances
#else
    info.instances = INSTANCES;
#endif
    info.instanceSize = INSTANCE_SIZE;
    info.overrideSize = OVERRIDE_SIZE;
    info.peakInstances = PeakInstances;

    return info;
}



