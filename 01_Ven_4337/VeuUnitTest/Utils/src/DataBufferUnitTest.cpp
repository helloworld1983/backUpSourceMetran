#include "EnableTestCase.h"
#ifdef UTIL_UNIT_TEST

#include "DataBuffer.h"
#include "gtest.h"
#include "gmock.h"
#include "UtilFixture.h"

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

using namespace ::testing;

class DataBufferUnitTest : public UtilTestFixture
{
public:
    DataBufferUnitTest(void) : UtilTestFixture(new BaseModuleMock)
{
}
    virtual ~DataBufferUnitTest(void){}
    virtual void SetUp(void){}
    virtual void TearDown(void){}
};

//test for DataBuffer::DataBuffer(USHORT size)
void DataBuffer(USHORT size)
{

    USHORT i;
    DataBuf = new LONG[size];

    for(i=0;i<size;i++)
    {
        DataBuf[i] = 0;
    }

    DataSize = 0;
    Index = 0;
    MaxValue = 0;
    MinValue = 0;

    //Restore size of DataBuf
    BufferSize = size;
}

TEST_F(DataBufferUnitTest, DataBufferUnitTest_DataBuffer)
{
    DataBuffer(100);

    EXPECT_EQ(0, DataSize);
    EXPECT_EQ(0, Index);
    EXPECT_EQ(0, MaxValue);
    EXPECT_EQ(0, MinValue);
    EXPECT_EQ(100, BufferSize);

    EXPECT_NE((void*)0, DataBuf);
    delete DataBuf;
}

//test for void DataBuffer::InsertNewValue(LONG value)
void InsertNewValue(LONG value)
{

    if ( DataSize < BufferSize )
        DataSize++;

    DataBuf[Index] = value;
    Index++;
    Index = Index % BufferSize;
}

TEST_F(DataBufferUnitTest, DataBufferUnitTest_InsertNewValue)
{
    DataBuffer(10);
    EXPECT_NE((void*)0, DataBuf);

    InsertNewValue(1);
    EXPECT_EQ(1, DataBuf[0]);

    InsertNewValue(2);
    EXPECT_EQ(1, DataBuf[0]);
    EXPECT_EQ(2, DataBuf[1]);

    delete DataBuf;
}

//test for void DataBuffer::ClearDataBuffer(void)
void ClearDataBuffer(void)
{
    USHORT i;

    for(i=0;i<DataSize;i++)
    {
        DataBuf[i] = 0;
    }

    DataSize = 0;
    Index = 0;
}

TEST_F(DataBufferUnitTest, DataBufferUnitTest_ClearDataBuffer)
{
    DataBuffer(10);
    EXPECT_NE((void*)0, DataBuf);
    ClearDataBuffer();

    EXPECT_EQ(0, DataSize);
    EXPECT_EQ(0, Index);

    delete DataBuf;
}

//test for LONG DataBuffer::FindMaxValue(void)
LONG FindMaxValue(void)
{
    LONG max;

    max = DataBuf[0];

    for(USHORT i=1;i < DataSize;i++)
    {
        if(max<DataBuf[i])
        {
            max=DataBuf[i];
        }
    }

    return max;
}
LONG FindMinValue(void)
{
    LONG min;

    min=DataBuf[0];

    for(USHORT i=1;i < DataSize;i++)
    {
        if(min>DataBuf[i])
        {
            min=DataBuf[i];
        }
    }

    return min;

}

TEST_F(DataBufferUnitTest, DataBufferUnitTest_FindMaxValue)
{
    DataBuffer(10);
    EXPECT_NE((void*)0, DataBuf);

    for(int i = 0; i < BufferSize; ++i)
    {
        InsertNewValue(i);
    }

    EXPECT_EQ(0, FindMinValue());
    EXPECT_EQ(9, FindMaxValue());

    delete DataBuf;
}
#endif  //end define UTIL_UNIT_TEST
