/******************************************************************************/
//$COMMON.CPP$
//   File Name:  CoreStream.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To provide basic in-core (i.e. memory) formatting that mimics 
//      the string I/O library syntax. All built in data types are supported 
//      except floating point which is currently not being used on the project. 
//
//      This class will not error if the caller overruns the buffer. It will
//      just stop accepting characters. 
//
//      The endl and ends manipulators are supported. 
//
//   Interfaces: None
//
//   Restrictions: None
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      02    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      01    07/28/97  Lafreniere          Morph
//      Description: Initial Revision
//
/******************************************************************************/

#include "CoreStream.h"

const size_t CoreStream::SHORT_LENGTH = 8;
const size_t CoreStream::LONG_LENGTH = 12;
const size_t CoreStream::CHAR_LENGTH = 5;
const size_t CoreStream::FORMAT_LENGTH = 12;

const LONG CoreStream::basefield = dec | hex | oct;
const LONG CoreStream::adjustfield = left | right | showbase | uppercase | 
    showpos;
LONG CoreStream::x_flags = dec | right;

//*****************************************************************************/
//    Operation Name: CoreStream()
//
//    Processing: The CoreStream class constructor. The user must supply the 
//      memory for the buffer. 
//
//    Input Parameters:
//      *buf - a pointer to the buffer where the formatted output will be 
//          placed. 
//      size - the size of the buffer.
//      open_mode - specifies how to treat the buffer upon opening.
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
CoreStream::CoreStream(CHAR* buf, size_t size, open_mode mode) : SIZE(size)
{
    buffer = buf;
    x_width = 0;

    // if not using out mode, set the buffer to the end of the string
    if (mode != out)
    {
        ULONG start = (ULONG)&buf[0];
        ULONG end = (ULONG)strchr(buf, char('\0'));
        bufferPos = end - start;
    }
    else
        bufferPos = 0;
}

//*****************************************************************************/
//    Operation Name: operator<<()
//
//    Processing: An overloaded operator << which takes all built in types
//      except floating point which is not presently being used on the 
//      project.
//
//    Input Parameters:
//      depends on overloaded version.
//
//    Output Parameters: None
//
//    Return Values:
//      CoreStream& - a reference to the CoreStream class type.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
CoreStream& CoreStream::operator<< (UCHAR uch)
{
    CHAR str[CHAR_LENGTH];

    sprintf(str, "%c", uch);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(uch);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (const CHAR* pch)
{
    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(pch);
    EnableInt();

    memcpy(&buffer[pos], pch, len);
    return *this;
}

CoreStream& CoreStream::operator<< (SHORT s)
{
    CHAR str[SHORT_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format), s);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (USHORT us)
{
    CHAR str[SHORT_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format, UNSIGNED), us);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (int i)
{
    CHAR str[LONG_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format), i);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (unsigned int ui)
{
    CHAR str[LONG_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format, UNSIGNED), ui);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (LONG l)
{
    CHAR str[LONG_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format), l);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

CoreStream& CoreStream::operator<< (ULONG ul)
{
    CHAR str[LONG_LENGTH];
    CHAR format[FORMAT_LENGTH];

    sprintf(str, Format(format, UNSIGNED), ul);

    DisableInt();
    size_t pos = bufferPos;
    size_t len = Length(str);
    EnableInt();

    memcpy(&buffer[pos], str, len);
    return *this;
}

//*****************************************************************************/
//    Operation Name: setf()
//
//    Processing: Alters those format bits specified by 1s in field. 
//      The new values of those format bits are determined by the corresponding 
//      bits in setbits. It returns a long that contains the previous value of 
//      all the flags.
//
//    Input Parameters:
//      setbits - Format flag bit values. To combine flags, use the bitwise OR 
//          ( | ) operator.
//      field - Format flag bit mask.
//
//    Output Parameters: None
//
//    Return Values:
//      LONG - the previous flag settings.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
LONG CoreStream::setf(LONG setbits, LONG field)
{
    LONG oldflags; 

    DisableInt();
    oldflags = x_flags; 
    x_flags = (setbits & field) | (x_flags & (~field)); 
    EnableInt();

    return oldflags; 
}

//*****************************************************************************/
//    Operation Name: setf()
//
//    Processing: Turns on only those format bits that are specified by 1s in 
//      setbits. It returns a long that contains the previous value of all the 
//      flags.
//
//    Input Parameters:
//      setbits - Format flag bit values. To combine flags, use the bitwise OR 
//          ( | ) operator.
//
//    Output Parameters: None
//
//    Return Values:
//      LONG - the previous flag settings.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
LONG CoreStream::setf(LONG setbits)
{
    LONG oldflags; 

    DisableInt();
    oldflags = x_flags; 
    x_flags |= setbits; 
    EnableInt();

    return oldflags;
}

//*****************************************************************************/
//    Operation Name: unsetf()
//
//    Processing: Clears the format flags specified by 1s in lFlags. It returns 
//      a long that contains the previous value of all the flags.
//
//    Input Parameters: 
//      setbits - bit(s) to unset.
//
//    Output Parameters: None
//
//    Return Values:
//      LONG - Format flag bit values. 
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
LONG CoreStream::unsetf(LONG setbits)
{
    LONG oldflags; 

    DisableInt();
    oldflags = x_flags; 
    x_flags &= (~setbits); 
    EnableInt();

    return oldflags; 
}

//*****************************************************************************/
//    Operation Name: Format()
//
//    Processing: Builds up a format string for numerals to be used with 
//      sprintf() based upon the stream options. 
//
//      The format is based upon the following sprintf() format. No floating
//      point is supported (see C manual for sprintf() options).
//
//      % [flags] [width] conversion
//
//    Input Parameters: 
//      *format - a pointer to the format string.
//      s - indicates whether the number is signed or unsigned. 
//
//    Output Parameters:
//      *format - the format will be stored where the pointer points.
//
//    Return Values:
//      CHAR* - in addition to the output, returns back the formated string 
//          pointer. 
//
//    Pre-Conditions: None
//
//    Miscellaneous: Just like the real Standard I/O library, the width field
//      is only valid for one insertion, then reverts to 0. 
//
//    Requirements:  
//
//*****************************************************************************/
CHAR* CoreStream::Format(CHAR* format, sign s)
{
    format[0] = NULL;

    // % - all formats must start with a % sign
    strcat(format, "%");

    // flags - append the necessary flags based upon flag options
    if (left & x_flags)
        strcat(format, "-");    
    if (showpos & x_flags)
        strcat(format, "+");    
    if (showbase & x_flags)
        strcat(format, "#");    

    // width - if user selected a width then append number
    if (x_width != 0) 
    {
        CHAR width[LONG_LENGTH];
        sprintf(width, "0%d", x_width);
        strcat(format, width);
        x_width = 0;
    }

    // conversion - set conversion field based upon flag options
    if (dec & x_flags)
        if (s == SIGNED)
            strcat(format, "d");
        else
            strcat(format, "u");
    else if (x_flags & hex)
        if (uppercase & x_flags)
            strcat(format, "X");
        else
            strcat(format, "x");
    else if (oct & x_flags)
        strcat(format, "o");

    return format;
}

//*****************************************************************************/
//    Operation Name: Length()
//
//    Processing: If the string to insert can fit within the buffer, then 
//      return the size of string. Else we need to cut back the size to fit 
//      within the buffer. If buffer is full, 0 will be returned.
//
//      The bufferPos is also updated here. 
//
//    Input Parameters:
//      insertStr - the string to insert into the output buffer.
//
//    Output Parameters: None
//
//    Return Values:
//      size_t - the number of characters the output buffer can take from 
//          the insertStr.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
size_t CoreStream::Length(const CHAR* insertStr)
{
    size_t insertSize = strlen(insertStr);

    DisableInt();
    size_t len = bufferPos + insertSize < SIZE ? insertSize : SIZE-bufferPos;
    bufferPos += len;
    EnableInt();

    return len;
}

//*****************************************************************************/
//    Operation Name: Length()
//
//    Processing: If the char to insert can fit within the buffer, then 
//      return the size of 1, else can't fit size so 0 returned.
//
//      The bufferPos is also updated here. 
//
//    Input Parameters:
//      insertChar - the char to insert into the output buffer.
//
//    Output Parameters: None
//
//    Return Values:
//      size_t - the number of characters the output buffer can take from 
//          the insertChar, either 0 or 1.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
size_t CoreStream::Length(const CHAR insertChar)
{
    DisableInt();
    size_t len = bufferPos < SIZE ? 1 : 0;
    bufferPos += len;
    EnableInt();

    return len;
}

