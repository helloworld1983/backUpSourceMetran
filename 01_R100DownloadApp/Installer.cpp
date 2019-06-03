//$COMMON.CPP$
//   File Name:  Installer.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  provides the framework to download an image of the
//     Esprit firmware to a ventilator flash eprom 
//
//   Interfaces:
//       1) Get PrivateProfileString is used to obtain the list of languages
//          from the "esprit.ini" file.
//       2) The registry on the PC is used to save the default port so that
//          the next time setup is called, the same port will be selected that
//          was used last time
//       3) CommPort is used to open a connection with the ventilator
//
//   Restrictions:
//       1)  Install should not be called until it is verified that sync
//           characters are being received from the ventilator
//
//   Revision History:
//
//      Rev:  Date:     Engineer:         Project:
//      01    07/17/98  Erik Blume        Morph
//      Description: Initial Revision
//
//      02	  04/23/99  Tom Kadien		  Morph
//		Description: add set serial number
/******************************************************************************/


#include "stdafx.h"
#include "_Setup.h"
#include "Installer.h"
#include "Command.h"
#include "CRC.h"
#include <afxpriv.h>
#include <winreg.h>
#include <winnetwk.h>
#include <shlobj.h>
#include <regstr.h>
#include "SetupUtilities.h"
#include "DownloadProtocol.h"
#include "dlgGetSerialNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// MSVC 4.0 and 4.1 have T2A. MSVC 4.2 has T2CA instead.
#if _MFC_VER < 0x0420
#define T2CA T2A
#endif

// Declare Private Constants --------------------------------------------------

const TCHAR g_szApplication[] = _T("Application");
const TCHAR g_szSystem[] = _T("System");
const TCHAR g_szWin32[] = _T("Win32");
const TCHAR g_szANSI[] = _T("ANSI");
const TCHAR g_szUNICODE[] = _T("UNICODE");
const TCHAR g_szWindows[] = _T("Windows");
const TCHAR g_szWinNT[] = _T("WinNT");
const LONG VENT_STATUS_TABLE_SIZE = 32;
const LONG EQUAL = 0;
const SHORT HALF_SECOND = 500; // 500 milliseconds
const LONG MAX_WAIT_FOR_ACK = 4000;   // 4 seconds
const ULONG MAX_FLASH_BURNING_TIME = 60000; // 1 minute

const SHORT MAX_DATA_RESEND = 100;

//$COMMON.ATTRIBUTE$
//  Name: REMOTE_CMD
//  Description:  Commands to get ventilator into remote mode
//                and ready for serial number.
//  Units: None
//  Range: n/a
const LPSTR REMOTE_CMD[] = {"#RMOTE1767\r", "#RMTON275C\r", "#PCNFG9EC0\r"} ;

//$COMMON.ATTRIBUTE$
//  Name: REMOTE_ACK
//  Description:  Responses from ventilator for remote mode
//                and serial number commands (see REMOTE_CMD).
//  Units: None
//  Range: n/a
const LPSTR REMOTE_ACK[] = {"?ENABLA9BE\r", "?RMTENDB58\r", "?PCNFGC2C2\r"} ;

//$COMMON.ATTRIBUTE$
//  Name: ERROR_ACK
//  Description:  Ventilator error response message
//  a reply.
//  Units: None
//  Range: n/a
const LPSTR ERROR_ACK = "?ERROR," ;

//$COMMON.ATTRIBUTE$
//  Name: SET_SN_FORMAT1
//  Description:  Header for set ventilator serial number command
//  a reply.
//  Units: None
//  Range: n/a
const LPSTR SET_SN_FORMAT1 = "#CDATA";

//$COMMON.ATTRIBUTE$
//  Name: SET_SN_FORMAT2
//  Description:  Tail for set ventilator serial number command
//  a reply.
//  Units: None
//  Range: n/a
const LPSTR SET_SN_FORMAT2 = ",701-0100-00,0" ;

//$COMMON.ATTRIBUTE$
//  Name: SET_SN_ACK
//  Description:  Ventilator Serial number programmed OK response
//  a reply.
//  Units: None
//  Range: n/a
const LPSTR SET_SN_ACK ="?CFGOK9A91\r" ;


//$COMMON.ATTRIBUTE$
//  Name: MAX_WAIT_FOR_REPLY
//  Description:  The maximum amount of time (in milliseconds) to wait for
//  a reply.
//  Units: None
//  Range: n/a
const LONG MAX_WAIT_FOR_REPLY = 2000; // 2 Seconds

struct ErrMap
{
   USHORT VentCode;
   UINT ResourceID;
};

//the following constants should not conflict with constants
//   defined in DownloadProtocol.h since they are used to identify unique
//   constants defined for providing status between the ventilator
//   and the server

const USHORT 	eFlashBurningTimeout=12;
const USHORT    eUnknownStatus = VENT_STATUS_TABLE_SIZE;

//a map to provide string message equivalents for status constants
//  which are sent from the ventilator to the server
static ErrMap ErrMsgs[]  = 
{
    //{	eVentInfoReceived,  }
    //{	eVentInfoSending,   } 
    //{	eDataRecvOkay,      }
    //{	eCommError,         }
    //{	eWaitSerialNoInput, }
    { eFlashMemWriteError, IDS_FLASH_MEM_WRITE_ERROR},//_T("Flash Memory Write failed.") },
    { eCheckSumError,      IDS_ERR_CHECKSUM}, //_T("Download data block checksum failed.") },
    { eCRCError,           IDS_ERR_CRC}, //_T("Flash image CRC validation failed.")},
    { eBlockNumOutOfSync,  IDS_BLOCK_NUM_OUTOF_SYNC}, //_T("Data transmission block number out of sync.")},
    { eFlashEraseError,    IDS_ERR_FLASH_ERASE}, //_T("Flash memory could not be successfully erased.")},
    { eFlashBurning,       IDS_FLASH_BURNING}, //_T("Burning download image ....")},
    { eDownloadSuccess,    IDS_DOWNLOAD_SUCCESS}, //_T("Download completed successfully.")},
    { eUnknownStatus,      0}
};



UINT VentStatus[VENT_STATUS_TABLE_SIZE];

BOOL m_bFactorySerialNo ;
BOOL m_bAnySerialNo ;
BOOL m_bSerialNoOnly ;
//*****************************************************************************/
//    Operation Name: CInstaller::CInstaller()
//
//    Processing:  constructor
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

CInstaller::CInstaller() :
    m_activePort(NULL),
    m_pProgDlg(NULL),
    NDataCorruption(0),
    NPackAttempts(0),
    m_commPortName(""),
    m_downloadFile("")
{
    // Calculate directory names to platform specific files
    switch(GetOSVersionInfo().dwPlatformId)
    {
        case VER_PLATFORM_WIN32_WINDOWS:
            m_strOSChar = g_szANSI;
            m_strOSWidth = g_szWin32;
            m_strOSPlatform = g_szWindows;
            break;
        case VER_PLATFORM_WIN32_NT:
            m_strOSChar = g_szUNICODE;
            m_strOSWidth = g_szWin32;
            m_strOSPlatform = g_szWinNT;
            break;
        default:
            ASSERT(0);
    }
    //fill in message table so that it can be accessed using enumeration
    //   as an index into the message table
    int index = 0;
    USHORT id;
    int i;
    for ( i=0; i < VENT_STATUS_TABLE_SIZE; i++)
        VentStatus[i] = 0;
    for (i=0; i < LANG_BUFLEN; i++)
        m_languages[i] = 0;
    while ( (id = ErrMsgs[index].VentCode)  != eUnknownStatus)
    {
        VentStatus[id] = ErrMsgs[index].ResourceID ;
        index++;
    }
}


//*****************************************************************************/
//    Operation Name: CInstaller::~CInstaller()
//
//    Processing: 
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

CInstaller::~CInstaller()
{
}

//*****************************************************************************/
//    Operation Name: CInstaller::SetSerialNumber()
//
//    Processing:  Sets ventilator serial number
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  eSuccess, eError
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

UINT CInstaller::SetSerialNumber()
{
	UINT uiLoopCnt ;
    char Response[40];

	CdlgGetSerialNumber dlgSetupSn ;
	if ( IDCANCEL == dlgSetupSn.DoModal(m_bFactorySerialNo) )
	{
		return eSuccess;
	}

	if ( 0 == strlen( dlgSetupSn.GetSerialNo()) )
	{
		return eSuccess;
	}

	for (uiLoopCnt = 0; uiLoopCnt < sizeof ( REMOTE_CMD ) / sizeof ( REMOTE_CMD[0]) ;
		  ++uiLoopCnt)
	{
		if (m_activePort->WriteData(REMOTE_CMD[uiLoopCnt], strlen(REMOTE_CMD[uiLoopCnt])) !=
			(LONG) strlen(REMOTE_CMD[uiLoopCnt]))
		{
			DisplayStatus(IDS_ERR_SEND_WRITE);
			return(eError);
		}

		for ( int i=0; i < sizeof(Response) / sizeof(char); ++i)
			Response[i] = 0;

		//packet was sent successfully; now wait for handshake
		while(m_activePort->ReadData( (unsigned char *) Response, strlen(REMOTE_ACK[uiLoopCnt]),
							HALF_SECOND) != (LONG) strlen(REMOTE_ACK[uiLoopCnt]))
		{
			DisplayStatus(IDS_ERR_SEND_NORESPONSE);
			return(eError);
		}
		
		// check to see if already in remote mode
		if ( (0 == uiLoopCnt) && (0 == strncmp(Response, ERROR_ACK, strlen(ERROR_ACK))) )
		{
			++ uiLoopCnt ;
		}
		else
		{
			if (0 != strcmp(Response, REMOTE_ACK[uiLoopCnt]) )
			{
				DisplayStatus(IDS_ERR_SEND_NORESPONSE);
				return(eError);
			}
		}
		// clear any extra data from port
		m_activePort->ReadData( (unsigned char *) Response, sizeof(Response), HALF_SECOND ) ;
	}

	// create the serial number set command
	CString SendSerialNo;
	SendSerialNo = SET_SN_FORMAT1;
	SendSerialNo += dlgSetupSn.GetSerialNo() ;
	SendSerialNo += SET_SN_FORMAT2;

	UINT uiCRC;
	uiCRC = CRC::S_CompCRC16(0, (unsigned char*)(LPCTSTR) SendSerialNo, SendSerialNo.GetLength() ) ;
	
	CString SerialCRC ; 
	SerialCRC.Format("%4.4X\r", uiCRC);
	SendSerialNo += SerialCRC ;

	if (m_activePort->WriteData((void *)(LPCTSTR) SendSerialNo, SendSerialNo.GetLength()) !=
		(LONG) SendSerialNo.GetLength())
	{
		DisplayStatus(IDS_ERR_SEND_WRITE);
		return(eError);
	}
	
	while(m_activePort->ReadData((UCHAR *) Response, strlen(SET_SN_ACK),
						MAX_WAIT_FOR_ACK) != (LONG) strlen(SET_SN_ACK))
	{
		DisplayStatus(IDS_ERR_SEND_NORESPONSE);
		return(eError);
	}

	if (0 != strcmp(Response, SET_SN_ACK) )
	{
		DisplayStatus(IDS_ERR_SEND_NORESPONSE);
		return(eError);
	}
	
	return eSuccess;
}


//*****************************************************************************/
//    Operation Name: CInstaller::GetFirstLanguage()
//
//    Processing:  search the Esprit.ini file for a list of languages that are
//       included with the distribution.  
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values: 
//       the name of the first language in the list of supported languages
//
//    Pre-Conditions: NONE
//
//    Miscellaneous: 
//      The Esprit.ini file is expected to be found in the current working
//      directory.
//
//    Requirements:  N/A
//
//*****************************************************************************/

CString CInstaller::GetFirstLanguage()
{
    m_nextLanguageIndex = 0;

    //retrieve the language keys in the .ini file
    m_languageStringSize = GetPrivateProfileString
    ( 
        _T("LANGUAGES"), 
        // points to section name 
 
        NULL, //LPCTSTR lpKeyName, 
        // points to key name 
 
        _T("mmicpu"), //LPCTSTR lpDefault, 
        // points to default string 
 
        m_languages, //LPTSTR lpReturnedString, 
        // points to destination buffer 
 
        LANG_BUFLEN , //DWORD nSize, 
        // size of destination buffer 
 
        _T(".\\Calliope.ini") //LPCTSTR lpFileName 
        // points to initialization filename 
 
    ); 
    SetNextLanguageIndex(); //advance the pointer to the next language in the
                            //   list

	TRACE(" GetFirstLanguage = %s \n", m_languages);

    return CString(m_languages);
}

//*****************************************************************************/
//    Operation Name: CInstaller::GetNextLanguage()
//
//    Processing:   get the next language key in the list of languages
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  
//       next language in the list
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

CString CInstaller::GetNextLanguage() 
{
    int beginIndex = m_nextLanguageIndex;
    SetNextLanguageIndex();

    return CString(&m_languages[beginIndex]);
}

//*****************************************************************************/
//    Operation Name: CInstaller::SetNextLanguageIndex()
//
//    Processing:  increment the index into the char array until it points to 
//       the beginning of the name of the next language in the list
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::SetNextLanguageIndex()
{
    while 
        (
            m_languages[m_nextLanguageIndex] != 0 && 
            m_nextLanguageIndex < m_languageStringSize
        ) 
        m_nextLanguageIndex++;
    ASSERT(m_languages[m_nextLanguageIndex]  == 0);
    if (m_languages[m_nextLanguageIndex + 1] != 0)
        m_nextLanguageIndex++;
}

//*****************************************************************************/
//    Operation Name: CInstaller::SetLanguage()
//
//    Processing:  open the image file indicated by the key string input
//       parameter.  The image file name is kept in the Esprit.ini file  keyed
//       by the language name.
//
//    Input Parameters:
//       languageText : name of the language
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::SetLanguage(CString languageText)
{
    m_language = languageText;
	TRACE(" SetLanguage in put = %s \n", m_language);

    int stringSize = 0;
    const DWORD buflen = 64;
    _TCHAR downloadFileBuf[buflen];
    //now retrieve the download file name from the .ini file
    stringSize = GetPrivateProfileString
    ( 
        _T("LANGUAGES"), 
        // points to section name 
 
        m_language, //LPCTSTR lpKeyName, 
        // points to key name 
 
        _T("Morph.abs"), //LPCTSTR lpDefault, 
        // points to default string 
 
        downloadFileBuf, //LPTSTR lpReturnedString, 
        // points to destination buffer 
 
        buflen , //DWORD nSize, 
        // size of destination buffer 
 
        _T(".\\Calliope.ini") //LPCTSTR lpFileName 
        // points to initialization filename 
 
    ); 
	TRACE(" > %s \n", downloadFileBuf);

    if (stringSize > 0)
        m_downloadFile = downloadFileBuf;
    else
        m_downloadFile = "";

}

//*****************************************************************************/
//    Operation Name: CInstaller::GetDefaultLanguage()
//
//    Processing:  return the first language key in the list of languages
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  
//       name of the first language in the list of language keys
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

CString CInstaller::GetDefaultLanguage() const
{
    return CString(m_languages);
}

//*****************************************************************************/
//    Operation Name: CInstaller::SetDownloadFile()
//
//    Processing:  set the download file to use
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::SetDownloadFile(CString downloadFile)
{
    m_downloadFile = downloadFile;
}




//*****************************************************************************/
//    Operation Name: CInstaller::Install()
//
//    Processing:  send the image file block by block to the ventilator
//
//    Input Parameters: 
//       pprogdlg : pointer to the progess dialog to send status reports
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: 
//       1) the comm port should have already been opened
//       2) the existence of the download file should have already been 
//           verified
//       3) the connection with the ventilator should have been verified
//          by receiving at least one sync character
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::Install(CSetupProgressDlg* pprogdlg)
{
    UINT statusID;
    E_ReturnStatus result;
    m_pProgDlg = pprogdlg;

    PRE_CONDITION(DownloadFile.Opened() == eTrue);
    PRE_CONDITION(m_activePort != NULL);
    ASSERT(m_commPortName != "");

    //begin the conversation with ventilator
    DisplayStatus(IDS_COMM_PORT_OPEN);  


	result = AcceptRequest() ;
    if (result != eSuccess)
    {
        AbortWithError(IDS_ERR_COMM_PORT_NOT_OPEN); 
    }     
    DisplayStatus(IDS_DOWNLOADING); //_T("Firmware download in progress"));

    if ( VerifyDownloadFile() == eError )
    {
        AbortWithError(IDS_ERR_FILE_NOT_OPEN); 
    } 


    if ( SendDownloadInfo() == eError )
    {
        //_T("Failure occurred while transmitting firmware image description." ));
        AbortWithError(IDS_ERR_SEND_IMAGE_INFO); 
    }

    while( (statusID = SendDataBlock()) != IDS_DOWNLOAD_SUCCESS )
    {
        if ( IsPortOpen() == eFalse )
        {
            // _T("Communication port closed unexpectedly") );
            AbortWithError(IDS_ERR_UNEXPECTED_PORT_CLOSURE); 
            break;
        }
        if ( statusID != IDS_SEND_OK)
        {
            AbortWithError(statusID);
            break;
        }
    }
    if ( statusID == IDS_DOWNLOAD_SUCCESS )
    {
        USHORT status = WaitForVentComplete();
        if (status == eDownloadSuccess)
        {
 			//_T("Download Operation Successful"));
         	DisplayStatus(IDS_DOWNLOAD_COMPLETE);
        }
        else
        {
        	DisplayVentStatus(status);
        }
        CloseDownloadFile();
    }
}

//*****************************************************************************/
//    Operation Name: CInstaller::DisplayStatus()
//
//    Processing:  write status information to the progress dialog
//
//    Input Parameters:
//        szStatus : the status message
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::DisplayStatus(const TCHAR* szStatus)
{
    if (m_pProgDlg != NULL)
        m_pProgDlg->SetStatus(szStatus);
}

void CInstaller::DisplayStatus(UINT nID)
{
    CString message;
    if (message.LoadString(nID))
    {
        if (m_pProgDlg != NULL)
            m_pProgDlg->SetStatus(message);
    }
}   


//******************************************************************************/
//    Operation Name: VerifyDownloadFile()
//
//    Processing:   A download image file is checked to make sure it is 
//        compatible with the ventilator.
// 
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: eSuccess -  the download file is compatible with the 
//                                OTP Part Number of the ventilator
//
//                   eError -    the download file was not
//                               compatible with the ventilator 
//
//    Pre-Conditions: 
//        1)  the Ventilator header information should have already been
//            filled in
//        2)  the download file should already have been successfully opened
//
//    Miscellaneous: None.
//
//    Requirements: None.  
//
/******************************************************************************/
E_ReturnStatus
CInstaller::VerifyDownloadFile(void)
{
    VersionInfo fileHeader;
    //MorphImageHeader fileHeader;
    int i;

    PRE_CONDITION(DownloadFile.Opened() == eTrue);


    if ( DownloadFile.Read((UCHAR  *) &fileHeader, 0, sizeof(fileHeader))
                != sizeof(fileHeader))
    {
        CString message;
        if (message.LoadString(IDS_ERR_FILE_HEADER))
            AfxMessageBox(message); //_T("Error reading image file header"));
        AfxThrowUserException( );   
        return eError;
    }
    if ( strcmp(Ventilator.OTPInfo.OTPPartNumber, fileHeader.OTPInfo.OTPPartNumber) != EQUAL )
    {
        const CHAR *pRel = fileHeader.OTPInfo.OTPPartNumber;
        CHAR *pProm = Ventilator.OTPInfo.OTPPartNumber;
        CString message;
        if ( pRel && pProm )
        {
            message.Format(IDS_INCOMPATIBLE_VERSION,(LPCTSTR) pRel,(LPCTSTR) pProm);
        }
        else
        {
            message.LoadString(IDS_INCOMPATIBLE_RELEASE);
        }
        AfxMessageBox(message);
        AfxThrowUserException( );   

        return(eError);
    } 
    CurrentBlock = 1;
    LastBlockSent = 0;
    Ventilator.MagicCookie = fileHeader.MagicCookie;

    for (i = 0; i < PART_NUMBER_SIZE; i++)
    {
        Ventilator.PartNumber[i] = fileHeader.PartNumber[i];
        Ventilator.OTPInfo.OTPPartNumber[i] = fileHeader.OTPInfo.OTPPartNumber[i];
    }

    for (i = 0; i < VERSION_SIZE; i++)
    {
        Ventilator.Version[i] = fileHeader.Version[i];
        Ventilator.OTPInfo.OTPVersion[i] = fileHeader.OTPInfo.OTPVersion[i];
    }

    Ventilator.TotalFileBlocks = fileHeader.TotalFileBlocks;
    Ventilator.TotalBytes  = fileHeader.TotalBytes;
    Ventilator.ImageCRCValue    = fileHeader.ImageCRCValue;
    Ventilator.OTPInfo.ImageCRCValue = fileHeader.OTPInfo.ImageCRCValue;
    m_pProgDlg->SetCountBytesToCopy(fileHeader.TotalFileBlocks * DOWNLOAD_PACKET_SIZE);
    return(eSuccess);
}


//*****************************************************************************/
//    Operation Name: CInstaller::OpenDownloadFile()
//
//    Processing: open the download file
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  
//       eSuccess :  file opened successfully
//       eError :   file could not be opened
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

E_ReturnStatus
CInstaller::OpenDownloadFile(void)
{

    PRE_CONDITION(DownloadFile.Opened() == eFalse);


    if ( DownloadFile.Open(m_downloadFile) == eSuccess)
    {
        return eSuccess;
    }
    else
    {
        CString message;
        message.Format(IDS_ERR_LANG_FILE_NOT_OPEN, (LPCTSTR) m_downloadFile);
        AfxMessageBox(message);
        return(eError);
    }
}

//******************************************************************************/
//    Operation Name: ReSyncCommState()
//
//    Processing: Resynchronize the download communication protocol state.
//                Throw away any left-over data from the previous session.
//                This can happen at any state of a download process.
//                Therefore, the method has to make sure that a receiving
//                Vent gets sync up to the stop command.
//                Throw away all the data already in the serial port
//                buffer so far.
//                
//                   
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values:
//
//    Pre-Conditions: 
//
//    Miscellaneous:  
//
//    Requirements: None.  
//
/******************************************************************************/
void 
CInstaller::ReSyncCommState()
{
    UCHAR command;

    // Get rid of all the accumulated data.
    while ( m_activePort->ReadData(&command, 1) == 1 );

}

//*****************************************************************************/
//    Operation Name: CInstaller::CancelOperation()
//
//    Processing:  close the download file and download communication protocol 
//        state
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void
CInstaller::CancelOperation() 
{
    CloseDownloadFile();
    ReSyncCommState();
}


//******************************************************************************/
//    Operation Name: SendDownloadInfo()
//
//    Processing:  Send a Download information packet back to the connected
//                 Ventilator. The information packet contains the Ventilator
//                 Serial number for which the image is valid, 
//                 download file size in blocks and the download
//                 image CRC value. Before an actual Vent information data 
//                 packet is transmitted, the method accepts a Vent info request
//                 message and sends a reply message to make sure that the
//                 Ventilator is in sync with the download server.
//                   
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: 
//             eSuccess - when the method successfully sends all the
//                        download information to the connected ventilator.
//
//             eError -  when the method fails to send all the necessary
//                       download information.  
//
//    Pre-Conditions:  (Downloadfile.opened())
//
//    Miscellaneous: When a ventilator information package is sent to a Vent, 
//              the checksum value of the ventilator information package is kept 
//              within the last byte of the packet structure, therefore the 
//              checksum byte should not be included in the checksum calculation.
//
//    Requirements: None.  
//
/******************************************************************************/
E_ReturnStatus
CInstaller::SendDownloadInfo(void)
{
    UCHAR command[40];
    UCHAR ventChkSum;
    DataAckPacket ackMsg;

   
    PRE_CONDITION(DownloadFile.Opened());

    ackMsg.eMsg = eVentInfoSending;
    if ( m_activePort->WriteData(&ackMsg, sizeof(ackMsg))!= sizeof(ackMsg))
    {
        CString message;
         //_T("Failure occurred while transmitting message for ventilator \
         //to expect download image information message.")
        message.LoadString(IDS_ERR_HANDSHAKE);
        AfxMessageBox(message);
        AfxThrowUserException( );   
    }
    if (m_activePort->ReadData(command, Command::S_GetInfoRequestLen(), MAX_WAIT_FOR_REPLY) 
            != Command::S_GetInfoRequestLen()) 
    {
        CString message;
        message.LoadString(IDS_ERR_ACKNWLEDGEMENT);
        //_T("Failure occurred while attempting to read an acknowledgement \
        //for server to proceed with info packet."));
        AfxMessageBox(message); 
        AfxThrowUserException( );   
    }

    command[Command::S_GetInfoSendLen()] = '\0';
    if ( strncmp( Command::S_GetInfoRequest(),(const char *) &command[0],
        Command::S_GetInfoSendLen()) != EQUAL )
    {
        CString message;
        message.Format(IDS_ERR_WRONG_MESSAGE, (LPCTSTR) command);
       //_T("A download information request message was expected from the 
       //  ventilator. %s was received instead."),
        AfxMessageBox(message);
        AfxThrowUserException( );   
    }
    
    ventChkSum = CRC::S_CompCheckSum((UCHAR *) &Ventilator, sizeof(Ventilator) );
    if ( m_activePort->WriteData(&Ventilator, sizeof(Ventilator)) 
        != sizeof(Ventilator))
    {
        CString message;
        //_T("Failure occurred while transmitting info packet to ventilator."));
        if (message.LoadString(IDS_ERR_TRANSMITTING_INFO))
        	AfxMessageBox(message); 
        AfxThrowUserException();
    }
    if ( m_activePort->WriteData(&ventChkSum, 1) != 1)
    {
        CString message;
        if (message.LoadString(IDS_ERR_SENDING_CK_SUM))
        //_T("Failure occurred while sending checksum to ventilator."));
        AfxMessageBox(message); 
        AfxThrowUserException();
    }
    if ( m_activePort->ReadData((UCHAR *)&ackMsg, sizeof(ackMsg), 3*MAX_WAIT_FOR_REPLY) != sizeof(ackMsg))
    {
        CString message;
        //_T("Ventilator didn't acknowledge the download info message"));
        if (message.LoadString(IDS_ERR_INFO_NOT_ACKNOWLEDGED))
            AfxMessageBox(message); 
        AfxThrowUserException( );   
    }
    if ( ackMsg.eMsg != eVentInfoReceived )
    {
        CString message;
        //_T("Ventilator gave inappropriate acknowledgement to dowload info message."));
        if (message.LoadString(IDS_ERR_WRONG_ACK))
            AfxMessageBox(message); 
        AfxThrowUserException();
    }
    return(eSuccess);
}

//******************************************************************************/
//    Operation Name: SendDataBlock()
//
//    Processing:  Send a block of download image to the connected Ventilator.
//                   A block of download image is read from the download image
//                   file and sent down to the ventilator.
//                   Each data block transmitted should be acknowledged by the
//                   ventilator.
//                   
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values:
//
// 
//
//    Pre-Conditions:  (Downloadfile.opened())
//
//    Miscellaneous: None.
//
//    Requirements: None.  
//
/******************************************************************************/
//#define NUM_BLOCK_DELAY 20
#define NUM_BLOCK_DELAY 60
UINT
CInstaller::SendDataBlock(void)
{
    BlockAckPacket ackMsg;
    USHORT i = 0;

    // return if all of the blocks have already been sent
    if ( CurrentBlock > Ventilator.TotalFileBlocks )
    {
        m_pProgDlg->SetProgress(Ventilator.TotalFileBlocks * DOWNLOAD_PACKET_SIZE);
        return(IDS_DOWNLOAD_SUCCESS);
    }
    else if (CurrentBlock < NUM_BLOCK_DELAY)
    {
        Sleep(HALF_SECOND); // Give the vent a little time to erase the flash
    }


    if ( CurrentBlock != LastBlockSent )
    {
        PacketBuf.SyncChar = 'X';

        CString message;
        message.Format(IDS_DOWNLOADING_BLOCK,CurrentBlock, Ventilator.TotalFileBlocks);
        //_stprintf(msg, _T("Downloading Block: %d of %d"), CurrentBlock, Ventilator.TotalBlocks);
        DisplayStatus(message);
 
        if ( DownloadFile.Read((UCHAR *) PacketBuf.Data, DOWNLOAD_PACKET_SIZE  )
            !=  DOWNLOAD_PACKET_SIZE )
        {
            DisplayStatus(IDS_ERR_IO); //_T("Download file I/O error"));
            return(IDS_ERR_IO);
        }
        PacketBuf.CRC = CRC::S_CompCRC16(0, (UCHAR *)PacketBuf.Data,
                 DOWNLOAD_PACKET_SIZE);
        PacketBuf.BlockNum = CurrentBlock;
        LastBlockSent = CurrentBlock;
    }
    else
    {
        CString message;
        message.Format(IDS_ERR_RESENDING,CurrentBlock, NDataCorruption, NPackAttempts); 
        //_stprintf(msg, _T("Resending Block: %d   Corrupted Attempts: %d    Total Attempts: %d"), 
        //        CurrentBlock, NDataCorruption, NPackAttempts);
        if (m_pProgDlg != 0)
            m_pProgDlg->SetProgress(0);
        DisplayStatus(message);
    }

    //keep track of how many times this packet was transmitted
    NPackAttempts++;     

    //packet was not sent successfully
    if (m_activePort->WriteData(&PacketBuf, sizeof(DataPacket)) != sizeof(DataPacket))
    {
        DisplayStatus(IDS_ERR_SEND_WRITE);
        return(IDS_ERR_SEND_WRITE);
    }

    //packet was sent successfully; now wait for handshake
    while(m_activePort->ReadData((UCHAR *)&ackMsg, sizeof(ackMsg), MAX_WAIT_FOR_ACK)
         != sizeof(ackMsg))
    {
        DisplayStatus(IDS_ERR_SEND_NORESPONSE);
        return(IDS_ERR_SEND_NORESPONSE);
    }

    //the ack message was received for the current block so this block is finished
    if ((ackMsg.eMsg==eDataRecvOkay)&&(ackMsg.BlockNum == CurrentBlock ))
    {
        if ( CurrentBlock%10 == 0 ) // every ten blocks.
        {
            m_pProgDlg->ReportProgress(DOWNLOAD_PACKET_SIZE * 10);
        }
        CurrentBlock++;
        NDataCorruption = 0;   // Got through, so reset error count
        NPackAttempts = 0;
        return(IDS_SEND_OK);
    }

    //ack message was received but there is a checksum error; resend the data
    if ( (ackMsg.eMsg == eCheckSumError)&&(ackMsg.BlockNum == CurrentBlock ) )
    {
        DisplayVentStatus(eCheckSumError);
        if ( NDataCorruption++ > MAX_DATA_RESEND )
        {
            CString message;
            message.Format(IDS_FAILED_SENDING_BLOCK,CurrentBlock, NDataCorruption);
            //_stprintf(msg, _T("Failed Sending Block %d after %d attempts"), CurrentBlock, NDataCorruption);
            DisplayStatus(message);
            NDataCorruption = 0;
            return(IDS_ERR_SEND_EXCEEDED);
        }
        return(IDS_SEND_OK); // Let's try to send the same block again.
    }
    //ack message was received, but it was for the wrong block or some other 
    //   error occurred

    DisplayVentStatus((USHORT )ackMsg.eMsg);
    if ( NPackAttempts++ > MAX_DATA_RESEND)
    {
        CString message;
        message.Format(IDS_FAILED_SENDING_BLOCK, CurrentBlock, NPackAttempts);
        //_stprintf(msg, _T("Failed Sending Block %d after %d attempts"), CurrentBlock, NPackAttempts);
        DisplayStatus(message);
        return(IDS_ERR_SEND_EXCEEDED);
    }
    return(IDS_SEND_OK);   // Let's try to send the same block again...
}

//*****************************************************************************/
//    Operation Name: CInstaller::IsPortOpen()
//
//    Processing: 
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

E_Bool CInstaller::IsPortOpen()
{
    if (m_activePort == NULL) return eFalse;
    else
    {
        return m_activePort->Opened();
    }
}

//******************************************************************************/
//    Operation Name: GetErrMsg(eDownloadStatus eCode)
//
//    Processing:  Get an error message defined for a given error status
//                 code. The method searches through the error message
//                 table for a given error code. 
//                   
//    Input Parameters: eCode - an error status code
//
//    Output Parameters: None.
//
//    Return Values:
//       pointer to an error message string
//
//    Pre-Conditions: 
//
//    Miscellaneous: Performance this routine is important.
//
//    Requirements: None.  
//
/******************************************************************************/
CString
CInstaller::GetErrMsg(USHORT eCode)
{
    LONG i = 0;
    CString message;
    if (VENT_STATUS_TABLE_SIZE  > eCode && VentStatus[eCode] != 0 )
    {
        message.LoadString(VentStatus[eCode]);
    }
    else
    {
        message.LoadString(IDS_UNDEF_ERROR_MESSAGE);
    }
    return message;
}

//*****************************************************************************/
//    Operation Name: CInstaller::DisplayVentStatus()
//
//    Processing: prefix the error message
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void
CInstaller::DisplayVentStatus(USHORT eCode)
{
    CString message;
    message.Format(IDS_VENTILATOR_REPORT,(LPCTSTR) GetErrMsg(eCode));
    DisplayStatus(message);
}

//******************************************************************************/
//    Operation Name: WaitForVentComplete()
//
//    Processing : Wait for a Vent to send a message that indicates all the
//        downloaded data has been successfully burnt to  flash memory.
//        While burning the flash memory, the connected Vent sends
//        a download image burning status every second. If a Vent encounters
//        an error condition while it is buring the flash memory, it sends an 
//        error status back to the Download Server.
//        This method can take up to 5 minutes.
//    
//                   
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: eDownloadSuccess -  When the connected Vent sends a download 
//                                       success message. 
//                   eCommError  - the ventilator detected a serial 
//                                 communication error.
//                   eCrcError   - the image that is burnt into the flash
//                                 memory doesn't match the download image CRC.
//                   eFlashMemWriteError - ventilator detected a flash write
//                                         error
//                   eFlashEraseError -	the ventilator detected an error while
//                                      attempting to erase flash memory
//                   eFlashBurningTimeout -  this is an internal message which
//                                           signifies that the ventilator did
//                                           not send an eDownloadSuccess 
//                                           message within the allotted time
//    Pre-Conditions:  
//
//    Miscellaneous: This method updates the status message on a main download
//                   view as it receives a download status message from a Vent.
//
//    Requirements: None.  
//
/******************************************************************************/
USHORT
CInstaller::WaitForVentComplete(void)
{
    DataAckPacket ackMsg;
    LONG totalWait = 0;

    for(totalWait; totalWait < MAX_FLASH_BURNING_TIME; totalWait+= HALF_SECOND)
    {

        if(m_activePort->ReadData((UCHAR *) &ackMsg, sizeof(ackMsg)) == sizeof(ackMsg))
        {
            if ( ackMsg.eMsg == eFlashBurning )
            {
                DisplayVentStatus(eFlashBurning);
                continue;
            }
            else if ( ackMsg.eMsg == eDownloadSuccess )
            {
                DisplayVentStatus(eDownloadSuccess);
                return(eDownloadSuccess);
            }
            DisplayVentStatus((USHORT) ackMsg.eMsg);
            return((USHORT ) ackMsg.eMsg);
        }
        //_T("Waiting for firmware image to be burned into flash memory."));
        DisplayStatus(IDS_WAIT_FOR_BURNING);
        Sleep(HALF_SECOND);
    }
    //burning flash was not successful within the timeout period : Abort
    AbortWithError(IDS_ERR_FLASHTIMEOUT);
    return(eFlashBurningTimeout);
}

//******************************************************************************/
//    Operation Name: AcceptRequest()
//
//    Processing:  Accept a download request from a connected Ventilator.
//                   The method polls for synchronization character from the
//                   connected ventilator. If it receives a sync character, it
//                   sends a download request message and expects for a download
//                   okey reply. The method receives a ventilator serial number
//                   and the port id.
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: eSuccess - when the method successfully establishes
//                                a download request from a connected ventilator.
//                   eError -  when the method fails to establish a download
//                               request due to a communication error or no
//                               Ventilator is requesting.
//
//    Pre-Conditions:  
//        1)  assumes that a progress dialog exists to send status updates to
// 		  2)  a connection with the ventilator has already been verified.
//            The PC is receiving sync characters.
//
//    Miscellaneous: None.
//
//    Requirements: None.  
//
//******************************************************************************/
E_ReturnStatus
CInstaller::AcceptRequest(void)
{
    const int COMMAND_BUFFER_SIZE = 40;
    UCHAR command[COMMAND_BUFFER_SIZE];
    UCHAR infoCheckSum;
    CHAR *reply = Command::S_GetReply();
    USHORT size;
    UCHAR ventChkSum;
    DataAckPacket ackMsg;
    UCHAR  syncChar = ASCII_SYNC;
    UCHAR *bPtr;

    for (int i =0; i < COMMAND_BUFFER_SIZE; i++)
        command[i] = 0;

    PRE_CONDITION(m_activePort != NULL);
    PRE_CONDITION(m_pProgDlg != NULL);

    //allow the progress dialog a chance to check for the
    //   cancel button
    if (m_pProgDlg != NULL)
        m_pProgDlg->ReportProgress(0);

    //allow the progress dialog a chance to check for the
    //   cancel button
    if (m_pProgDlg != NULL)
        m_pProgDlg->ReportProgress(0);
        

    //read a "sync" character from the ventilator
    //  note that VerifyCommConnection should have already been called before
    //  calling AcceptRequest
	long byteWritten = 0;


    if ( m_activePort->ReadData(&command[0], 1, 3500) != 1  )
    {
        AbortWithError(IDS_ERR_INITIAL_SYNC);
    }


    //write a "sync" character to the ventilator
    m_activePort->WriteData(&syncChar, 1);

    //send a command the ventilator ("MR")
    if ( m_activePort->WriteData(Command::S_GetReply(), Command::S_GetReplyLen())
        != Command::S_GetReplyLen()) 
    {
        //_T("Failure sending a ventilator information request message"));
        AbortWithError(IDS_ERR_SENDING_REQUEST); 
    }
    
    //send InfoRequest message to ventilator ("SR")
    if  ( m_activePort->WriteData
            (
                Command::S_GetInfoRequest(), 
                Command::S_GetInfoRequestLen()
            )
        != Command::S_GetInfoRequestLen()
        ) 
    {
        //_T("Failed in sending a ventilator information request message"));
        AbortWithError(IDS_ERR_SENDING_REQUEST);
        return(eError);
    }
   
    // get the size of the request message from ventilator
    size = Command::S_GetRequestLen();

  	//the initial character from the ventilator could either be another
    //   sync character or the first character of a command
    if ( command[0] == syncChar )
    {
        bPtr  = command;
    }
    else
    {
        bPtr = &command[1];
        size -= 1;
    }

    //get a request command from the ventilator  ("MP")
    if ( m_activePort->ReadData(bPtr, size, MAX_WAIT_FOR_ACK) != size )
    {
        AbortWithError(IDS_ERR_COMM_PORT_READ);
    }
    
    //make sure a valid request was received 
    if (strncmp(Command::S_GetRequest(), (CHAR *) &command[0], Command::S_GetRequestLen())  != EQUAL )
    {
        command[Command::S_GetRequestLen()]= '\0';
        CString message;
        message.Format(IDS_ERR_WRONG_REQUEST_MESSAGE,command);
        AbortWithError(message);
    }
     
//    m_activePort->ReadData( (UCHAR *) &command ,COMMAND_BUFFER_SIZE, MAX_WAIT_FOR_ACK)  ;

		//get an acknowledgement from the ventilator
    if ( m_activePort->ReadData( (UCHAR *) &ackMsg, sizeof(ackMsg), MAX_WAIT_FOR_ACK) != sizeof(ackMsg))
    {
        AbortWithError(IDS_ERR_NO_INFO_REQUEST); 
    }

    if (ackMsg.eMsg != eVentInfoSending)
    {
        AbortWithError(IDS_ERR_BADREQUESTACK); 
    }

    // get the version info
    if ( m_activePort->ReadData( (UCHAR *) &Ventilator, sizeof(Ventilator), MAX_WAIT_FOR_REPLY) 
        != sizeof(Ventilator))
    {
        //_T("Failed to receive a complete Ventilator information"));
        AbortWithError(IDS_ERR_NO_VENTINFO); 
    }

    // read a check sum from the ventilator
    if ( m_activePort->ReadData( (UCHAR *) &ventChkSum, 1, MAX_WAIT_FOR_ACK) != 1)
    {
        //_T("Failed to receive Ventilator Info pack checksum"));
        AbortWithError(IDS_ERR_NO_CK_SUM); 
    }

    // calculate a checksum from the data and compare with the checksum sent by
    //   the ventilator
    infoCheckSum = CRC::S_CompCheckSum((UCHAR *) &Ventilator, sizeof(Ventilator));
    if ( infoCheckSum != ventChkSum )
    {
        //_T("Ventilator Info packet Checksum error"));
        AbortWithError(IDS_ERR_INFO_CK_SUM);
    }

    //send an acknowledgment that the VersionInfo data was received from ventilator
    ackMsg.eMsg = eVentInfoReceived;
    if ( m_activePort->WriteData(&ackMsg, sizeof(ackMsg))!= sizeof(ackMsg))
    {
        //_T("Failure sending a vent information ack message"));
        AbortWithError(IDS_ERR_SENDING_ACK);
    }

    return(eSuccess);
}

//*****************************************************************************/
//    Operation Name: CInstaller::VerifyCommConnection()
//
//    Processing: 
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

UINT
CInstaller::VerifyCommConnection(void)
{
    const int COMMAND_BUFFER_SIZE = 40;
    UCHAR command[COMMAND_BUFFER_SIZE];
    E_ReturnStatus result;
    UINT errorString;
    UCHAR  syncChar = ASCII_SYNC;
	const UINT MAX_COUNTER = 10;
	static UINT Counter = 0;


    ASSERT(m_commPortName != "");

    //zero out the command buffer
    for (int i =0; i < COMMAND_BUFFER_SIZE; i++)
        command[i] = 0;

    m_activePort = new CommPort(m_commPortName); 

    Command::S_InitCommandSizes();

    result = m_activePort->Init();
    if ( result != eSuccess )
    {
        delete m_activePort;
        m_activePort = NULL;
        return IDS_INITIALIZATION_ERROR;
    }
    errorString = m_activePort->Open();
    if ( errorString != IDS_OPEN_SUCCESS )
    {
        delete m_activePort;
        m_activePort = NULL;
        return errorString;
    }


	if( TRUE != m_bSerialNoOnly)
	{
		//send "forcedDownloadChar" character to target

		//read a "sync" character from the ventilator
		while ( 1 )
		{
			
			if ( m_activePort->ReadData(&command[0], 1, 3500) != 1)
			{
				ReSyncCommState();
			}
			else if (command[0] != syncChar) // Bad syschar receive.
			{
				//ReSyncCommState();
				m_activePort->Close(eTrue);
				delete m_activePort;
				m_activePort = NULL;
				return IDS_ERR_BADSYNC;
			}
			else							// Yes, the synchar was ther, so let them
											// that server want to download.
			{
				m_activePort->WriteData("MP", strlen("MP"));
				//ReSyncCommState();
				return IDS_SUCCESSFUL_CONNECTION;
			}
			++Counter;
			if (Counter == MAX_COUNTER)
			{
				m_activePort->Close(eTrue);
				delete m_activePort;
				Counter = 0;
				return IDS_ERR_HANDSHAKE;
			}

		}

		
	}
    return IDS_SUCCESSFUL_CONNECTION;
}

//*****************************************************************************/
//    Operation Name: CInstaller::AbortWithError()
//
//    Processing: 
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::AbortWithError( const TCHAR * szError)
{
    CancelOperation();
    AfxMessageBox(szError);
    AfxThrowUserException();
}

void CInstaller::AbortWithError( UINT nID)
{
    CString message;

    CancelOperation();
    if (message.LoadString(nID))
        AfxMessageBox(message);
    AfxThrowUserException();
}

//*****************************************************************************/
//    Operation Name: CInstaller::SetCommPortName()
//
//    Processing: set the default comm port name in the registry
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

void CInstaller::SetCommPortName(CString portName)
{
    m_commPortName = portName;
    const DWORD buflen = 64;
    _TCHAR newPort[buflen];
    _tcscpy(newPort,(LPCSTR) portName);
    HKEY espritRegHandle;
    DWORD disposition;
    LONG result;
    result =  RegCreateKeyEx
    ( 
        HKEY_CURRENT_USER,   // handle of an open key 
         
        _T("Software\\Metran\\M6_V850E\\DefaultPort"),  // address of subkey name 
        0, //DWORD Reserved, 
        _T("REG_SZ"), //LPTSTR lpClass : address of class string 
        REG_OPTION_NON_VOLATILE, //DWORD dwOptions :special options flag 
        KEY_ALL_ACCESS, //REGSAM samDesired : desired security access 
        NULL, //LPSECURITY_ATTRIBUTES :  address of key security structure 
        &espritRegHandle, //PHKEY phkResult : address of buffer for opened handle 
        &disposition  //LPDWORD lpdwDisposition : address of disposition value buffer  
    );
    if (result == ERROR_SUCCESS)
    { 
        result =  RegSetValueEx
        ( 
            espritRegHandle, //HKEY hKey : handle of key to set value for 
            _T("DefaultPortName"), //LPCTSTR lpValueName : address of value to set 
            NULL, //DWORD Reserved : reserved 
            REG_SZ, //DWORD dwType : flag for value type 
            (BYTE *) &newPort[0], //CONST BYTE *lpData : address of value data 
            portName.GetLength()  //DWORD cbData : size of value data 
        );
        ASSERT(result == ERROR_SUCCESS);
        result = RegCloseKey(espritRegHandle);
        ASSERT(result == ERROR_SUCCESS);
    }
}

//*****************************************************************************/
//    Operation Name: CInstaller::GetDefaultPortName()
//
//    Processing:  read the default port from the registry
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/

CString CInstaller::GetDefaultPortName()
{
    int stringSize = 0;
    const DWORD buflen = 64;
    DWORD returnDataSize = buflen;
    DWORD valueType;
    _TCHAR portName[buflen];
    _tcscpy(portName,_T("COM1"));
    //now retrieve the download file name from the .ini file
    HKEY espritRegHandle;
    DWORD disposition;
    LONG result;
    result =  RegCreateKeyEx
    ( 
        HKEY_CURRENT_USER,   // handle of an open key 
        _T("Software\\Metran\\M6_V850E\\DefaultPort"),  // address of subkey name 
        0, //DWORD Reserved, 
        _T("REG_SZ"), //LPTSTR lpClass : address of class string 
        REG_OPTION_NON_VOLATILE, //DWORD dwOptions :special options flag 
        KEY_ALL_ACCESS , //REGSAM samDesired : desired security access 
        NULL, //LPSECURITY_ATTRIBUTES :  address of key security structure 
        &espritRegHandle, //PHKEY phkResult : address of buffer for opened handle 
        &disposition  //LPDWORD lpdwDisposition : address of disposition value buffer  
    );
    if (result == ERROR_SUCCESS)
    { 
        if (disposition == REG_OPENED_EXISTING_KEY)  //try to read the value
        {
            result =  RegQueryValueEx
            ( 
                espritRegHandle, //HKEY hKey : handle of key to query 
                _T("DefaultPortName"), //LPTSTR lpValueName : address of name of value to query 
                NULL, //LPDWORD lpReserved, 
                &valueType, //LPDWORD lpType :  address of buffer for value type 
                (LPBYTE) &portName[0], // lpData : address of data buffer 
                &returnDataSize //LPDWORD lpcbData : address of data buffer size 
            );
            ASSERT(result == ERROR_SUCCESS);
        }
        else  if (disposition == REG_CREATED_NEW_KEY)  //store a default port name
        {
            _tcscpy(portName,_T("COM1"));

            result =  RegSetValueEx
            ( 
                espritRegHandle, //HKEY hKey : handle of key to set value for 
                _T("DefaultPortName"), //LPCTSTR lpValueName : address of value to set 
                NULL, //DWORD Reserved : reserved 
                REG_SZ, //DWORD dwType : flag for value type 
                (BYTE *) &portName[0], //CONST BYTE *lpData : address of value data 
                buflen  //DWORD cbData : size of value data 
            ); 
            ASSERT(result == ERROR_SUCCESS);
        }
        result = RegCloseKey(espritRegHandle);
        ASSERT(result == ERROR_SUCCESS);
    }
    return portName;
}


