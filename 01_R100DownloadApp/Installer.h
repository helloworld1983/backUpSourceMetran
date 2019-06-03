//$COMMON.H$
//    File Name:  Installer.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CInstaller
//
//
//    Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/06/98  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  INSTALLER_H
#define  INSTALLER_H


//#include "VentInfoPacket.h"
#include "BootFile.h"
#include "CommPort.h"
#include "SetupProgressDlg.h"
#include "DownloadProtocol.h"
//#include "DataPacket.h"


const int LANG_BUFLEN = 512;
extern BOOL m_bFactorySerialNo ;
extern BOOL m_bAnySerialNo ;
extern BOOL m_bSerialNoOnly ;

class CSetupProgressDlg;

class CInstaller 
{
public:
    CInstaller();
    ~CInstaller();
    CString GetFirstLanguage();
    CString GetNextLanguage();
    CString GetDefaultLanguage() const;
    void SetLanguage(CString langText);
    void SetDownloadFile(CString filename);
    void Install(CSetupProgressDlg* pprogdlg);
    void SetCommPortName(CString portName);
	CString GetDefaultPortName();
    E_ReturnStatus OpenDownloadFile();
    inline void CloseDownloadFile() { DownloadFile.Close(); }
    UINT VerifyCommConnection();
	UINT SetSerialNumber();


public:
    CString m_strOSChar;
    CString m_strOSWidth;
    CString m_strOSPlatform;

protected:
    //HINF m_infHandle;
    //INFCONTEXT m_infContext;
    int m_nextLanguageIndex;
    _TCHAR m_languages[LANG_BUFLEN];
    int m_languageStringSize;
    CString m_language;

    CString m_downloadFile;
    CSetupProgressDlg * m_pProgDlg;
    CommPort * m_activePort;
    CString m_commPortName;

protected:
    void DisplayStatus(const TCHAR* szStatus);
    void DisplayStatus(UINT nID);    //display status using a String resource
    void DisplayVentStatus(USHORT eCode);
    E_ReturnStatus AcceptRequest();
    E_ReturnStatus VerifyDownloadFile();
    void CancelOperation();
    void ReSyncCommState();
    CString GetErrMsg(USHORT eCode);
    USHORT WaitForVentComplete(void);

private:
//    VentInfo Ventilator;
    VersionInfo Ventilator;
    BootFile DownloadFile;
    DataPacket PacketBuf;
    void SetNextLanguageIndex();
    void AbortWithError( const TCHAR * szError);
    void AbortWithError( UINT nID);
    E_ReturnStatus SendDownloadInfo();
    UINT SendDataBlock();
    E_Bool IsPortOpen();
    USHORT CurrentBlock;
    USHORT LastBlockSent;
    SHORT NDataCorruption;
    SHORT NPackAttempts;
};

#endif //INSTALLER_INCLUDED

