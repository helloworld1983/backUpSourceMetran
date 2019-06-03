#ifndef INC_DATABUFFER_H_
#define INC_DATABUFFER_H_

#include "standard.h"

//Constructor
void DataBuffer(USHORT size);

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

// Find maximum Value in DataBuf
LONG FindMaxValue(void);

// Find minimum Value in DataBuf
LONG FindMinValue(void);

#endif /* INC_DATABUFFER_H_ */
