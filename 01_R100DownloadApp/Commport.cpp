//******************************************************************************/
//$COMMON.CPP$
//   File Name:  CommPort.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This class provides capability to communicate with a system
//         serial port. A port communication attributes can be modified.
//         A block of data bytes can be written and read in from an open
//         Communication port.
//
//   Interfaces: 
//     The CInstaller object sends and receives data using
//     a CommPort object.
//
//   Restrictions:
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    01/31/97  Jung Verheiden    	Morph
//      Description: Initial Revision
//
/******************************************************************************/

#define COMMPORT_CPP

#include "standard.h"
#include "stdafx.h"
#include "FMacro.h"
#include "resource.h"
#include "CommPort.h"
#include "Winbase.h"    //for port parameter definitions

#if defined(WINDOWS_NT)
extern "C"
{
BOOL  WINAPI 
ClearCommBreak(
    HANDLE hFile
    );

BOOL  WINAPI 

ClearCommError(
    HANDLE hFile,
    LPDWORD lpErrors,
    LPCOMSTAT lpStat
    );

 
BOOL  WINAPI 
SetupComm(
    HANDLE hFile,
    DWORD dwInQueue,
    DWORD dwOutQueue
    );


BOOL  WINAPI 
EscapeCommFunction(
    HANDLE hFile,
    DWORD dwFunc
    );


BOOL  WINAPI 
GetCommConfig(
    HANDLE hCommDev,
    LPCOMMCONFIG lpCC,
    LPDWORD lpdwSize
    );


BOOL  WINAPI 
GetCommMask(
    HANDLE hFile,
    LPDWORD lpEvtMask
    );


BOOL  WINAPI 
GetCommProperties(
    HANDLE hFile,
    LPCOMMPROP lpCommProp
    );


BOOL  WINAPI 
GetCommModemStatus(
    HANDLE hFile,
    LPDWORD lpModemStat
    );

BOOL  WINAPI 
WINAPI
GetCommState(
    HANDLE hFile,
    LPDCB lpDCB
    );

BOOL  WINAPI 
GetCommTimeouts(
    HANDLE hFile,
    LPCOMMTIMEOUTS lpCommTimeouts
    );

BOOL  WINAPI 
PurgeComm(
    HANDLE hFile,
    DWORD dwFlags
    );


BOOL  WINAPI 
SetCommBreak(
    HANDLE hFile
    );


BOOL  WINAPI 
SetCommConfig(
    HANDLE hCommDev,
    LPCOMMCONFIG lpCC,
    DWORD dwSize
    );

BOOL  WINAPI 
SetCommMask(
    HANDLE hFile,
    DWORD dwEvtMask
    );
BOOL  WINAPI 
WINAPI
SetCommState(
    HANDLE hFile,
    LPDCB lpDCB
    );

BOOL  WINAPI 
SetCommTimeouts(
    HANDLE hFile,
    LPCOMMTIMEOUTS lpCommTimeouts
    );


BOOL  WINAPI 
TransmitCommChar(
    HANDLE hFile,
    char cChar
    );


BOOL  WINAPI 
WaitCommEvent(
    HANDLE hFile,
    LPDWORD lpEvtMask,
    LPOVERLAPPED lpOverlapped
    );
};

#endif //defined(WINDOWS_NT)

static TCHAR ErrorBuf[256];

// Initialize static attributes of the CommPort class.
// Index into these static tables are defined in the PortDialog Settings
// Control list.

LONG    CommPort::S_BaudTable[]=
         {
            CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400,
            CBR_4800, CBR_9600, CBR_14400, CBR_19200, CBR_38400,
            CBR_56000, CBR_57600,CBR_115200, CBR_128000, CBR_256000
         } ;

UCHAR  CommPort::S_ParityTable[]=
         {
            NOPARITY, EVENPARITY, ODDPARITY, MARKPARITY, SPACEPARITY
         } ;
		 
UCHAR    CommPort::S_StopBitsTable[]=
         {
            ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
         } ;

UCHAR    CommPort::S_DataBits[]= { 8, 7 };

CHAR *CommPort::S_PortNames[] = {""};

const CHAR ASCII_XON = 0x11;
const CHAR ASCII_XOFF = 0x13;
const LONG MAX_WAIT_INTERVAL_FOR_READ_BYTE  = 300; // 300 ms
const LONG READ_CHAR_TIME_MULTIPLIER = 0; 
const LONG WRITE_CHAR_TIME_MULTIPLIER = 0; 
const LONG READ_BLOCK_TOTAL_WAITTIME = 1000; // one second.
const LONG WRITE_BLOCK_TOTAL_WAITTIME = 1000; // one second.

//******************************************************************************/
//    Operation Name: CommPort()
//
//    Processing:  The default CommPort class contructor.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None  
//
//******************************************************************************/
CommPort::CommPort
	(
		const TCHAR * portName,
		LONG baudRate, 
		UCHAR parity, 
		UCHAR byteSize, 
		UCHAR stopBits,
		BOOL   dtr,
		BOOL   rts,
		BOOL   xon
	) :
	BaudRate(baudRate),
	Parity(parity),
	ByteSize(byteSize),
	StopBits(stopBits),
	DTR(dtr),
	RTS(rts),
	Xon(xon),
	PortName(portName)
{
	PortId = (HANDLE) -1;
}

//******************************************************************************/
//    Operation Name: ~CommPort()
//
//    Processing:  The default CommPort class destructor.
//         It asserts if this port is not already closed before it gets
//         destroyed.
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None  
//
//******************************************************************************/
CommPort::~CommPort()
{
	ASSERT(Opened() == eFalse);	
}

//******************************************************************************/
//    Operation Name: SetupCommPort()
//
//    Processing:  This method sets up the communcation port settings.
//				   Sets up the DCB data strucutre based on this object
// 				   attributes and calls a SetCommState Windows API.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: eSuccess when setting this serial port configruation
//					 was successful.
//
//    Pre-Conditions: Opened()
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
E_Bool
CommPort::Setup()
{
	DCB dcb;
    
	PRE_CONDITION(Opened());
//    BuildCommDCB("baud=19200 parity=N data=8 stop=1",&dcb);
	
	GetCommState(PortId,&dcb);
 
	// Convert a Dialog list index values to a corresponding DCB values.
	//dcb.BaudRate = CURRENT_BAUDRATE;
    dcb.BaudRate = BaudRate;
	dcb.ByteSize = ByteSize;
    dcb.Parity = Parity;
    dcb.StopBits = StopBits;

	// Once it open this port is remain open.
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	// Do not monitor the the CTS signal.
	dcb.fOutxCtsFlow = eFalse;
	// Do not monitor the Data Set Ready signal.
    dcb.fOutxDsrFlow = eFalse;

	// Disable the Ready to send signal.
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

    // setup software flow control
    dcb.fInX = dcb.fOutX = Xon;
    dcb.XonChar = ASCII_XON ;
    dcb.XoffChar = ASCII_XOFF ;
    dcb.XonLim = 100 ;
    dcb.XoffLim = 100 ;

    // other various settings
    dcb.fBinary = TRUE ;
    dcb.fParity = TRUE ;

    return(( E_Bool) SetCommState( PortId, &dcb ));
}

//******************************************************************************/
//    Operation Name:Open()
//
//    Processing: 
//     Opens communication port with the port settings specified
//     in this object attribute values..
//
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: e_Succes : if port open operation was successful. 
//					 e_Error : if port open operation fails.
//
//    Pre-Conditions: (!Opened())
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

UINT
CommPort::Open()
{            
    COMMTIMEOUTS  CommTimeOuts ;

	PRE_CONDITION( !Opened() );

	const TCHAR * refix = "\\\\.\\";
	TCHAR *tempPort;

	sprintf(tempPort, "%s%s", refix, PortName);

   // open COMM device
	if ( (PortId = CreateFile(tempPort, GENERIC_READ | GENERIC_WRITE,
	               0,                    // exclusive access
	               NULL,                 // no security attrs
	               OPEN_EXISTING,
	               FILE_ATTRIBUTE_NORMAL | 
	               FILE_FLAG_OVERLAPPED, // overlapped I/O
	               NULL )) == (HANDLE) -1 )
	{
	   	return ( IDS_NO_PORT_HANDLE) ;
	}
	// get any early notifications
   // jhv debug, I need to add more code to handle all the serial IO by a
   // separate task thread. For the time being, just comment this out.
   //SetCommMask(PortId, EV_RXCHAR ) ;

   // setup device buffers
   SetupComm(PortId, 4096, 4096 ) ;

   // purge any information in the buffer
   PurgeComm(PortId, PURGE_TXABORT | PURGE_RXABORT |
                                   PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

   // set up for overlapped I/O
   CommTimeOuts.ReadIntervalTimeout = MAX_WAIT_INTERVAL_FOR_READ_BYTE;
   CommTimeOuts.ReadTotalTimeoutMultiplier = READ_CHAR_TIME_MULTIPLIER;
   CommTimeOuts.ReadTotalTimeoutConstant = READ_BLOCK_TOTAL_WAITTIME;
   CommTimeOuts.WriteTotalTimeoutMultiplier = WRITE_CHAR_TIME_MULTIPLIER;
   CommTimeOuts.WriteTotalTimeoutConstant = WRITE_BLOCK_TOTAL_WAITTIME;
   SetCommTimeouts(PortId, &CommTimeOuts ) ;
   if ( Setup() == eFalse )
   {
		CloseHandle(PortId);
		PortId = ( HANDLE) -1 ;
		return(IDS_CONFIGURATION_ERROR);
   }
   return(IDS_OPEN_SUCCESS);
} // end of Open()

//******************************************************************************/
//    Operation Name:Init()
//
//    Processing: 
//      One time initialization necessary for a serial port interface
//      work. The method creates a read and write OS interface events.
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: eSucces : if port initialization operation was successful. 
//					 eError : if port initialization operation fails.
//
//    Pre-Conditions: (!Opened())
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

E_ReturnStatus
CommPort::Init()
{            
    
	PRE_CONDITION( eFalse == Opened() );
	
	OsRead.Offset = 0;
	OsRead.OffsetHigh = 0;
    // create I/O event used for overlapped read
    if ( (OsRead.hEvent = CreateEvent( NULL,    // no security
                            TRUE,    // explicit reset req
                            FALSE,   // initial event reset
                            NULL ) ) // no name
							== NULL )
	{
		return(eError);
	}

	OsWrite.Offset = 0;
	OsWrite.OffsetHigh = 0;
    // create I/O event used for overlapped write
    if ( (OsWrite.hEvent = CreateEvent( NULL,    // no security
                            TRUE,    // explicit reset req
                            FALSE,   // initial event reset
                            NULL ) ) // no name
							== NULL )
	{
		return(eError);
    }

	return(eSuccess);
} // end of Init()

//******************************************************************************/
//    Operation Name: Close()
//
//    Processing: 
//      Closes the connection this Communciaiton port.
//
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: None.
//
//    Pre-Conditions: None.
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

void CommPort::Close(E_Bool final )
{
	if ( !Opened() )
		return;

   EscapeCommFunction( PortId, CLRDTR ) ;
   // purge any outstanding reads/writes and close device handle
   PurgeComm( PortId, PURGE_TXABORT | PURGE_RXABORT |
                                   PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
   if ( final )
   {
   		CloseHandle(OsWrite.hEvent);
   		CloseHandle(OsRead.hEvent);
   }
   CloseHandle( PortId ) ;
   PortId = (HANDLE) -1;

} // end of Close()


//******************************************************************************/
//    Operation Name: ReadData(CHAR *pBuf, int nMaxSize )
//
//    Processing: 
//     Reads a specified number of data bytes from this COM port into
//     the 'pBuf' memory area using the 'FileRead' interface.
//     If a given number of data bytes are not received within a timeout period,
//     it only returns a number of data bytes already received. 
//     The method returns -1 value when it cannot read data due to a communcation
//     port error.
//
//    Input Parameters: Data bytes are read in from this communication port.
//
//    Output Parameters: None.
//
//    Return Values: -1 when there is a communication error reported from
//      the lower level MS Windows routine.
//      Otherwise, it returns a number of data bytes read. 
//
//    Pre-Conditions: ( Opened() )
//
//    Miscellaneous:
//			Timeout specification on this object has been set up  when the port
//          was Opened.
//
//    Requirements:  
//
//******************************************************************************/

LONG CommPort::ReadData( UCHAR *pBuf, LONG nMaxLength, LONG waitTime )
{
	COMSTAT    ComStat ;
	ULONG      dwErrorFlags;
	ULONG      dwLength;
	ULONG       totalBytes, leftBytes;
	CHAR       szError[ 40 ] ;
	ULONG dwError;

	DEBUG_PRE_CONDITION( pBuf );

	if ( !Opened() )
		return(0);

	leftBytes = nMaxLength;
	totalBytes = 0;

	while (leftBytes > 0)
	{
		// only try to read number of bytes in queue 
		ClearCommError( PortId, &dwErrorFlags, &ComStat ) ;
		dwLength = leftBytes;
		if ( ReadFile( PortId, pBuf, dwLength, &dwLength, &OsRead ) == FALSE )
		{
			if ((dwError = GetLastError()) == ERROR_IO_PENDING) // timeout error
			{
				// We have to wait for read to complete. 
				// This function will timeout according to the
				// CommTimeOuts.ReadTotalTimeoutConstant variable
				// Every time it times out, check for port errors
				while(!GetOverlappedResult( PortId,  &OsRead, &dwLength, TRUE ))
				{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)
					{
						if ( waitTime <= 0 )
						{
							DisplayStatus("Data read Timeout");
							return(totalBytes);
						}
						else
						{
							waitTime -=  READ_BLOCK_TOTAL_WAITTIME;
							continue;
						}
					}
					else
					{
						_stprintf( szError, _T("Communication Port Error Code : <CE-%u>"), dwError ) ;
						DisplayStatus( szError ) ;
						ClearCommError( PortId, &dwErrorFlags, &ComStat ) ;
						if (dwErrorFlags > 0)
						{
							_stprintf( szError, _T("Timeout Error flag : <CE-%u>"), dwErrorFlags ) ;
							DisplayStatus( szError ) ;
						}
						return(-1);
					}
				}
			}
			else // other than a timeout error
			{
			    if ( dwError > 0 )
				{
					_stprintf( szError,_T("Error Code : <CE-%u>"), dwError ) ;
					DisplayStatus(  szError ) ;
					
				}
				return(-1);
			}
		}

		leftBytes -= dwLength;
		totalBytes += dwLength;
		pBuf += dwLength;
		waitTime -= READ_BLOCK_TOTAL_WAITTIME;
		if ( waitTime <= 0 )
			return(totalBytes);
	}
    return ( totalBytes ) ;
} // end of CommPort::ReadData()

//******************************************************************************/
//    Operation Name: WriteData(CHAR *pBuf, int nMaxSize )
//
//    Processing: 
//     Writes a specified number of data bytes to this COM port from the 'pBuf'
//     memory area using the 'FileWrite' interface.
//     If a given number of data bytes cannot be written out within a timeout period,
//     it only returns a number of data bytes already sent out. 
//     The method returns -1 value when it cannot write data due to a communcation
//     port error.
//
//    Input Parameters:  None.
//
//    Output Parameters: None.
//
//    Return Values: -1 when there is a communication error reported from
//      the lower level MS Windows routine. Otherwise, it returns a number of
//      data bytes sent out. 
//
//    Pre-Conditions: ( Opened() )
//
//    Miscellaneous:
//			Timeout specification on this object has been set up  when the port
//          was Opened.
//
//    Requirements:  
//
//******************************************************************************/
LONG CommPort::WriteData( void *lpByte , LONG dwBytesToWrite)
{

	ULONG      dwBytesWritten ;
	ULONG       dwErrorFlags;
	ULONG   	dwError;
	COMSTAT     ComStat;
	CHAR        szError[ 40 ] ;

 	if(!Opened()) return(0);

	if ( ! WriteFile( PortId, lpByte, dwBytesToWrite,
	                       &dwBytesWritten, &OsWrite ))
	{
	// Note that normally the code will not execute the following
	// because the driver caches write operations. Small I/O requests 
	// (up to several thousand bytes) will normally be accepted 
	// immediately and WriteFile will return true even though an
	// overlapped operation was specified

		if((dwError = GetLastError()) == ERROR_IO_PENDING)
		{
			// We should wait for the completion of the write operation
			// so we know if it worked or not
			// If the write takes long enough to complete, this 
			// function will timeout according to the
			// CommTimeOuts.WriteTotalTimeoutConstant variable.
			// At that time we can check for errors and then wait 
			// some more.

			while(!GetOverlappedResult( PortId, &OsWrite, &dwBytesWritten, TRUE ))
			{
				if(dwError == ERROR_IO_INCOMPLETE)
					continue;
				else
				{
					// an error occurred, try to recover
					if ( dwError > 0 )
					{
						_stprintf( szError, _T("<CE-%u>"), dwError ) ;
						DisplayStatus(szError);
						return(-1);
					}
					ClearCommError( PortId, &dwErrorFlags, &ComStat ) ;
					break;
				}
			}
		}
		else
		{
			// some other error occurred
			if ( dwError > 0 )
			{
				_stprintf( szError, _T("<CE-%u>"), dwError ) ;
				DisplayStatus(szError);
			}
			ClearCommError( PortId, &dwErrorFlags, &ComStat ) ;
			if (dwErrorFlags > 0)
			{
				_stprintf( szError, _T("<CE-%u>"), dwErrorFlags ) ;
				DisplayStatus(szError);
			}
			return ( -1 );
		}
	}
	return ( dwBytesWritten) ;
}	

//******************************************************************************/
//    Operation Name: DisplayStatus( CHAR *errMsg )
//
//    Processing:  
//        Format an error message string.
//        This is used for debugging only.
//
//    Input Parameters: 
//         errMsg : error message string
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
void CommPort::DisplayStatus(CHAR *errMsg) 
{ 
    _stprintf( ErrorBuf, _T("Communication Port Error: %s\n"),errMsg );

}
