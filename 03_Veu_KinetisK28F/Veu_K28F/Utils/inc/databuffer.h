//$COMMON.CPP$
//   File Name:  DataBuffer.h
//
//   Purpose: This file contains the class declaration for the DataBuffer Class
//
/******************************************************************************/
#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "standard.h"

class DataBuffer
{

public:
    //Constructor
    DataBuffer(USHORT size);
    ~DataBuffer(void);
    // Insert a new Value in DataBuf
    void InsertNewValue(LONG value);

    //clear data buffer
    void ClearDataBuffer(void);

    // Return maximum Value in DataBuf
    LONG GetMaxValue(void);

    // Return minimum Value in DataBuf
    LONG GetMinValue(void);

    // Return average Value in DataBuf
    LONG FindAverageValue(void);

protected:

private:

    // Find maximum Value in DataBuf
    LONG FindMaxValue(void);
    // Find minimum Value in DataBuf
    LONG FindMinValue(void);

    //$COMMON.ATTRIBUTE$
    //    Name: DataSize
    //    Description: Restore size of data in DataBuf
    //    Units:  None
    //    Range:  n/a
    USHORT DataSize;

    //$COMMON.ATTRIBUTE$
    //    Name: MaxValue
    //    Description: MaxValue and MinValue in DataBuf
    //    Units:  None
    //    Range:  n/a
    LONG MaxValue;

    //$COMMON.ATTRIBUTE$
    //    Name: MinValue
    //    Description: MinValue and MinValue in DataBuf
    //    Units:  None
    //    Range:  n/a
    LONG MinValue;

    //$COMMON.ATTRIBUTE$
    //    Name: DataBuf
    //    Description: pointer to dada buffer
    //    Units:  None
    //    Range:  n/a
    LONG * DataBuf;

    //$COMMON.ATTRIBUTE$
    //    Name: Index
    //    Description: Index point next the end Value in DataBuf
    //    Units:  None
    //    Range:  n/a
    USHORT Index;

    //$COMMON.ATTRIBUTE$
    //    Name: BufferSize
    //    Description: Restore size buffer when user initialize to used
    //    Units:  None
    //    Range:  n/a
    USHORT BufferSize;
};

#endif//if !define DATABUFFER_H
