/********************************************************************/
/*                                                                  */
/* File Name     : stringtoolsUnitTest.cpp                          */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
#include "math.h"
using ::testing::TestWithParam;


class stringToolUnitTest : public ::testing::Test
{
};

int stringtools_IntToAscii(long int a, char* buff, int numOfDigit)
{
    int index = 0;
    char temp;
    long int input = a;
    if (input == 0)
    {
        if(numOfDigit == 0) //not required number of digit
            buff[index++] = '0';    //default
        else
        {
            int j = 0;
            for(j = 0; j < numOfDigit; j++)
                buff[index++] = '0';
        }
        return index;
    }

    if(input < 0)
    {
        buff[index++] = '-';
        input = -input;
    }

    //convert number to string and store in temporary memmory
    char tempBuff[10] = {'0'};
    int i = 0;
    while (input)
    {
        temp = '0' + input % 10;
        tempBuff[i++] = temp;
        input /= 10;
    }

    //cope from temporary buffer to buffer output
    int k = i;
    if(numOfDigit > k)  //numOfDigit != 0
    {
        for(; k < numOfDigit; k++)
            buff[index++] = '0';
    }

    while(i)
        buff[index++] = tempBuff[--i];

    return index;
}

TEST_F(stringToolUnitTest, stringToolUnitTestCase)
{
    char mydata[] = {0};
    EXPECT_EQ(1, stringtools_IntToAscii(2, mydata, 1));
    EXPECT_EQ(2, stringtools_IntToAscii(15, mydata, 1));
    EXPECT_EQ(3, stringtools_IntToAscii(110, mydata, 1));
    EXPECT_EQ(3, stringtools_IntToAscii(255, mydata, 1));
}

void stringtools_Reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

TEST_F(stringToolUnitTest, stringtools_Reverse)
{
    char myData[2] = {'a', 'b'};
    char MyDataExpect[2] = { 'b', 'a'};
    stringtools_Reverse(myData, 2);
    EXPECT_EQ(MyDataExpect[0], myData[0]);
    EXPECT_EQ(MyDataExpect[1], myData[1]);

    char MyData1[5] = {'a', 'b', 'b', 'c', 'd'};
    char MyData1Expect[5] = {'d', 'c', 'b', 'b', 'a'};
    stringtools_Reverse(MyData1, 5);
    for(int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(MyData1[i], MyData1Expect[i]);
    }

}

int stringtools_IntToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    stringtools_Reverse(str, i);
    str[i] = '\0';
    return i;
}

TEST_F(stringToolUnitTest, stringtools_IntToStr)
{
    char myData[] = {0};
    EXPECT_EQ(2, stringtools_IntToStr(34, myData, 2));
    EXPECT_EQ(3, stringtools_IntToStr(150, myData, 2));

}

void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = stringtools_IntToStr(ipart, res, 1);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        stringtools_IntToStr((int)fpart, res + i + 1, afterpoint);
    }
}

TEST_F(stringToolUnitTest, ftoa)
{
    char Mydata[] = {0};
    char MyExpectData[5] = {'4', '5', '.', '7', '9'};

    ftoa(45.8, Mydata, 5);
    for(int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(Mydata[i], MyExpectData[i]);
    }
}

char g_bufferStringTool[10];
char* ConvertFCFlowToString(unsigned char value)
{
    //char *buffer = malloc(10);

    for(int i = 0; i < 10; i++)
    {
        g_bufferStringTool[i] = '\0';
    }

    float floatValue = (float)value/20;

    sprintf(&g_bufferStringTool[0],"%0.2f",floatValue);

    return g_bufferStringTool;
}

TEST_F(stringToolUnitTest, ConvertFCFlowToString)
{
    ConvertFCFlowToString(255);
    char MydataExpect[10] = {'1', '2', '.', '7', '5', '\0', '\0', '\0', '\0', '\0'};
    for(int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(MydataExpect[i], g_bufferStringTool[i]);
    }

    ConvertFCFlowToString(0);
    char MydataExpect1[10] = {'0', '.', '0', '0', '\0', '\0', '\0', '\0', '\0', '\0'};
    for(int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(MydataExpect1[i], g_bufferStringTool[i]);
    }

    char MyDataExpect2[10] = {'7', '.', '6', '5', '\0', '\0', '\0', '\0', '\0', '\0'};

    ConvertFCFlowToString(153);
    for(int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(MyDataExpect2[i], g_bufferStringTool[i]);
    }
}
