//*****************************************************************************/
//$COMMON.H$
//   File Name:  CoreStream.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: CoreStream
//
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      01    07/25/97  Lafreniere          Morph
//      Description: Initial Revision
//
//*****************************************************************************/

#ifndef CORESTREAM_H
#define CORESTREAM_H

#include "CommonTypes.h"
#include <stdio.h>
#include <string.h>

class CoreStream 
{
public:
    enum open_mode { 
        out       = 0x02,
        ate       = 0x04,
        app       = 0x08 };

    enum {  
	    left       = 0x0002,
	    right      = 0x0004,
	    dec        = 0x0010,
	    oct        = 0x0020,
	    hex        = 0x0040,
	    showbase   = 0x0080,
	    uppercase  = 0x0200,
	    showpos    = 0x0400 };

    static const LONG basefield;	
    static const LONG adjustfield;	

    CoreStream(CHAR* buf, size_t size, open_mode = out);
    ~CoreStream() { }

    inline CoreStream& operator<< (CHAR ch);
    CoreStream& operator<< (UCHAR uch);
    CoreStream& operator<< (const CHAR* pch);
    inline CoreStream& operator<< (const UCHAR* puch);
    CoreStream& operator<< (SHORT s);
    CoreStream& operator<< (USHORT us);
    CoreStream& operator<< (LONG l);
    CoreStream& operator<< (ULONG ul);
    CoreStream& operator<< (int i);
    CoreStream& operator<< (unsigned int ui);
    CoreStream& operator<< (CoreStream& (*fcn)(CoreStream&)) 
    { 
        return (*fcn)(*this);
    }

	inline CoreStream& seekp(size_t); 

	LONG setf(LONG setbits, LONG field);
	LONG setf(LONG);
	LONG unsetf(LONG);
	inline USHORT width() const;
	inline USHORT width(USHORT w);
    inline size_t pcount() const;

protected:
private:
    // prevent copying
    CoreStream(const CoreStream&);
    void operator=(CoreStream&);

    enum sign {
        SIGNED,
        UNSIGNED };

    CHAR* Format(CHAR* format, sign s = SIGNED);
    size_t Length(const CHAR* insertStr);
    size_t Length(const CHAR insertChar);

// methods for enable and disable of interrupts
#ifdef TEST_MODE
	void DisableInt() { }
	void EnableInt() { }
#else
	void DisableInt() { asm (" di"); }
	void EnableInt() { asm (" ei"); }
#endif

    //$COMMON.ATTRIBUTE$
    //  Name: buffer
    //  Description: A pointer to the in-core output buffer.
    //  Units: 
    //  Range: 
    CHAR* buffer;        
    
    //$COMMON.ATTRIBUTE$
    //  Name: bufferPos
    //  Description: The current buffer position.
    //  Units: 
    //  Range: 
    size_t bufferPos;
    
    //$COMMON.ATTRIBUTE$
    //  Name: x_flags
    //  Description: 
    //  Units: 
    //  Range: 
    static LONG x_flags;
    
    //$COMMON.ATTRIBUTE$
    //  Name: The minimum field width.
    //  Description: 
    //  Units: 
    //  Range: 
	USHORT x_width;
    
    //$COMMON.ATTRIBUTE$
    //  Name: state
    //  Description: Holds the state information for the class.
    //  Units: 
    //  Range: 
    USHORT state;

    const size_t SIZE;
    static const size_t SHORT_LENGTH;
    static const size_t LONG_LENGTH;
    static const size_t CHAR_LENGTH;
    static const size_t FORMAT_LENGTH;
};

inline CoreStream& CoreStream::operator<< (CHAR ch) 
{ 
    return *this << (UCHAR)ch; 
}
inline CoreStream& CoreStream::operator<< (const UCHAR* puch) 
{
    return *this << (const CHAR*)puch;
}

inline CoreStream& CoreStream::seekp(size_t pos) 
{ 
    if (pos > SIZE)
        bufferPos = SIZE; 
    else
        bufferPos = pos; 

    return *this;
}

inline CoreStream& endl(CoreStream& strm) { return strm << '\n'; }
inline CoreStream& ends(CoreStream& strm) { return strm << char('\0'); }
inline CoreStream& dec(CoreStream& strm) 
{ 
    strm.setf(CoreStream::dec,CoreStream::basefield); 
    return strm; 
}
inline CoreStream& hex(CoreStream& strm) 
{ 
    strm.setf(CoreStream::hex,CoreStream::basefield); 
    return strm;
}
inline CoreStream& oct(CoreStream& strm) 
{ 
    strm.setf(CoreStream::oct,CoreStream::basefield); 
    return strm;     
}
inline USHORT CoreStream::width() const { return x_width; }
inline USHORT CoreStream::width(USHORT w)
{
    USHORT oldwidth = x_width; 
    x_width = w; 
    return oldwidth;
}
inline size_t CoreStream::pcount() const { return bufferPos; }

#endif //CORESTREAM_H

