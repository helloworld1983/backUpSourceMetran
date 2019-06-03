//******************************************************************************/
//$COMMON.H$
//   File Name:  CommPort.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: CommPort
//
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    01/31/97    Jung Verheiden   			  Morph
//      Description: Initial Revision
//
//******************************************************************************/

#ifndef	COMMPORT_H
#define COMMPORT_H

#include "standard.h"
#include "FMacro.h"
#include "stdafx.h"
#include "winbase.h"  //for port parameter definitions

#define CURRENT_BAUDRATE CBR_38400

enum E_Baud 
{
	e110,
	e300,
	e600,
	e1200,
	e2400,
	e4800,
	e9600,
	e14400,
	e19200,
	e38400,
	e56000,
	e57600,
	e115200,
	e128000,
	e256000
};

enum E_Parity
{
	eNoParity,
	eEven,
	eOdd,
	eMark,
	eSpace
};

enum E_StopBits
{
	eOneStop,
	eOne5,
	eTwo
};

enum E_DataSize
{
	eEight,
	eServen
};

//enum e_port
//{
//	ecom1,
//	ecom2,
//	ecom3
//};


class CommPort
{
// Operations
public:
	CommPort
	(
		const TCHAR * portname = "COM1",
		LONG baudRate = CURRENT_BAUDRATE, 
		UCHAR parity = NOPARITY, 
		UCHAR byteSize	= 8, 
		UCHAR stopBits = ONESTOPBIT, 
		BOOL   dtr = eFalse,
		BOOL   rts = eFalse,
		BOOL   xon = eFalse
	);    
	virtual ~CommPort();
	E_ReturnStatus Init();
	UINT Open();	//return value is a string ID
	void Close(E_Bool  final=eFalse);
	inline E_Bool Opened() 
	{ 
		if (PortId == (HANDLE) -1)
			return eFalse;
		else
			return eTrue;
	}
	LONG WriteData( void  *pBuf, LONG size);
	LONG ReadData( UCHAR  *pBuf, LONG size, LONG waitTime = 0);
	void DisplayStatus(CHAR *pMsg);

protected:
// Implementation
	E_Bool  Setup();

private:
	static LONG    S_BaudTable[];
	static UCHAR    S_ParityTable[];
	static UCHAR    S_StopBitsTable[];
	static UCHAR    S_DataBits[];
	static CHAR *S_PortNames[];
	HANDLE PortHandle;
	LONG BaudRate;
	UCHAR Parity;
	UCHAR ByteSize;	   
	UCHAR StopBits;
	const TCHAR *PortName;
	BOOL   DTR;
	BOOL   RTS;
	BOOL   Xon;
	OVERLAPPED OsRead;
	OVERLAPPED OsWrite;
    HANDLE PortId;
};


#endif //COMMPORT_H
