/******************************************************************************/
//$COMMON.H$
//   File Name:  CircularQ.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the template class definition for a circular queue.
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      04    02/25/98  K. A. McGuigan      Morph
//      Description: SCR-258.  In Read() and Write(), changed the check to
//          determine if it is time to wrap around to the start of the queue.
//
//      Rev:  Date:     Engineer:           Project:
//      03    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      02    10/08/97  K.A. McGuigan       Morph
//      Description: Modified the Purge() operation so that only the WritePtr
//          changes.
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/18/97  K.A. McGuigan       Morph
//      Description: Initial Revision
/******************************************************************************/
#ifndef CIRCULARQ_H
#define CIRCULARQ_H
#include <memory.h>
#include "Standard.h"

/******************************************************************************/
//      CONSTANT DEFINITIONS
/******************************************************************************/


/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/


/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
template<class T> class CircularQ
{
    public:
        inline void Block(void) {IsBlocked = eTrue;}
        inline void UnBlock(void) {IsBlocked = eFalse;}

        CircularQ(SHORT size);

        E_ReturnStatus Read(T* toLoc);
        E_ReturnStatus Write(T* fromLoc);
        void Purge(void);


    protected:

    private:
        CircularQ(void);
        CircularQ(const CircularQ&);

        //$COMMON.ATTRIBUTE$
        //  Name: QSize
        //  Description:  Number of Queue Entries
        //  Units: None
        //  Range: n/a
        SHORT QSize;

        //$COMMON.ATTRIBUTE$
        //  Name: ElementSize
        //  Description:  Size of a single element
        //  Units: None
        //  Range: n/a
        SHORT ElementSize;

        //$COMMON.ATTRIBUTE$
        //  Name: QPtr
        //  Description:  Ptr to the first Q element
        //  Units: None
        //  Range: n/a
        T* QPtr;

        //$COMMON.ATTRIBUTE$
        //  Name: ReadPtr
        //  Description:   Ptr to the next value to be read
        //  Units: None
        //  Range: n/a
        T* ReadPtr;

        //$COMMON.ATTRIBUTE$
        //  Name: WritePtr
        //  Description:  Ptr to the next location to write
        //  Units: None
        //  Range: n/a
        T* WritePtr; 

        //$COMMON.ATTRIBUTE$
        //  Name: IsBlocked
        //  Description:  Indicates if writing to the queue is blocked.
        //  Units: None
        //  Range: n/a
        E_Bool IsBlocked;

};      


/******************************************************************************/
//      OPERATIONS
/******************************************************************************/


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CircularQ()
//
//    Processing: 
//      This operation is the constructor for the CircularQ object.  This
//      operation initializes the attributes of the class.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
template<class T>
CircularQ<T>::CircularQ(SHORT size) :
        QPtr(new T[size]), QSize(size)
{
    ElementSize = sizeof(T);
    ReadPtr = QPtr;
    WritePtr = QPtr;
    IsBlocked = eTrue;

}   // end CircularQ()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Read()
//
//    Processing: 
//      This operation checks if there are any entries in the queue.  If not,
//      it returns an error status.  If so, it returns copies the next element
//      into the location pointed to by the input parameter
//
//    Input Parameters:
//      toLoc - pointer to the location where the element read from the queue 
//              is placed
//
//    Output Parameters:
//      toLoc - pointer to the element read from the queue
//
//    Return Values:
//      E_ReturnStatus - eSuccess if an element was read; eError if the queue is
//          empty
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      It is left to the caller to insure that the return status is successful
//      before trying to use the data.
//
//    Requirements:  
//
/******************************************************************************/
template<class T>
E_ReturnStatus CircularQ<T>::Read(T* toLoc)
{
    E_ReturnStatus rtnValue;

    // Check if the queue is empty
    if (ReadPtr == WritePtr)
    {
        // Queue empty 
        rtnValue = eError;
    }
    else
    {
        // Queue not empty 
        rtnValue = eSuccess;        

        // Increment the read pointer to the next value to read
        ReadPtr++;
        
        // Wrap around if when off end of queue
        if (ReadPtr >= (QPtr + QSize))
        {
            ReadPtr = QPtr;
        }
    
        // Copy Data
        memcpy(toLoc, ReadPtr, ElementSize);
    
    }
    
    return(rtnValue);

}   // Read()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Write()
//
//    Processing: 
//      This operation checks if the queue is full.  If so, it returns an error
//      status.  If not, it writes the value pointed to by the fromLoc onto the
//      queue.
//
//    Input Parameters:
//      fromLoc - pointer to the element to be written to the queue
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_ReturnStatus - eSuccess if the value was successfully placed on the
//          queue; eError if the queue is full.
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      It is left to the caller to insure that the return status is successful
//
//    Requirements:  
//
/******************************************************************************/
template<class T>
E_ReturnStatus CircularQ<T>::Write(T* fromLoc)
{
    E_ReturnStatus rtnValue;
    T* tempPtr;

    // Only attempt a write if blocking is not active
    if (eFalse == IsBlocked)
    {    
        tempPtr = WritePtr + 1;

        // Wrap around if necessary
        if (tempPtr >= (QPtr + QSize))
        {
            tempPtr = QPtr;
        }

        // Check if Queue is full
        if (ReadPtr == tempPtr)
        {
            rtnValue = eError;
        }
        else
        {
            rtnValue = eSuccess;

            // Update the write pointer        
            WritePtr = tempPtr;
        
            // Copy Data
            memcpy(WritePtr, fromLoc, ElementSize);
    
        }

        return(rtnValue);
    }
    else
    {
        Purge();
        return(eSuccess);
    }        

}   // end Write()

 

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Purge()
//
//    Processing: 
//      This operation effectively purges the queue by setting the WritePtr
//      equal to the ReadPtr.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
template<class T>
void CircularQ<T>::Purge(void)
{
    WritePtr = ReadPtr;

}   // end Purge()

#endif  // CircularQ_H

