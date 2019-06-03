//$COMMON.CPP$
//   File Name:  _Setup.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: provides implementation of CSetupApp
//       CSetupApp initiates at "wizard" which guides the user
//       through the process of downloading software to the Esprit
//       ventilator
//
//   Interfaces:
//
//   Restrictions:
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      02    04/24/99  Thomas C. Kadien    Morph
//      Description: Add command line options -vs & -any 
//					 to enable set ventilator serial number
//					-any  allows any 9 alpha numeric combination
//					-vs   Serial number must begin with "VS"
//
//      Rev:  Date:     Engineer:           Project:
//      01    07/21/98  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/


#include "stdafx.h"
#include "_Setup.h"
#include "Installer.h"
#include "SetupWindow.h"
#include "SetupSheet.h"
#include "SetupProgressDlg.h"
#include "ExitSuccess.h"
#include "SetupUtilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CSetupApp, CWinApp)
    //{{AFX_MSG_MAP(CSetupApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    // No help file written yet
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CSetupApp theApp;	//The one and only CSetupApp object


//*****************************************************************************/
//    Operation Name: CSetupApp::CSetupApp()()
//
//    Processing: constructor
//
//    Input Parameters:	NONE
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

CSetupApp::CSetupApp()
{
    m_fExitCode = EC_FAILURE;
	m_bFactorySerialNo = FALSE ;
	m_bAnySerialNo = FALSE ;
	m_bSerialNoOnly = FALSE ;
}



//*****************************************************************************/
//    Operation Name: CSetupApp::InitInstance()
//
//    Processing: enable 3d controls and begin the setup 
//
//    Input Parameters:	NONE
//
//    Output Parameters: NONE
//
//    Return Values:  
//       allways returns False so that we exit the application.  The download
//       has been completed.
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  Checks command line option(s)
//
//    Requirements:  N/A
//
//*****************************************************************************/

BOOL CSetupApp::InitInstance()
{
#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    // Change title from _Setup for error messages, etc.
    if(m_pszAppName != NULL)
        free((char*)m_pszAppName);
    CString strAppTitle(MAKEINTRESOURCE(IDS_SETUPAPPLICATIONTITLE));
    m_pszAppName = _tcsdup(strAppTitle);

	if ( 0 == strcmp( m_lpCmdLine, "-vs") )
	{
		m_bFactorySerialNo = TRUE ;
	}

	if ( 0 == strcmp( m_lpCmdLine, "-any") )
	{
		m_bAnySerialNo = TRUE ;
	}

	if ( 0 == strcmp( m_lpCmdLine, "-snonly") )
	{
		m_bSerialNoOnly = TRUE ;
		m_bFactorySerialNo = TRUE ;
	}

    // Run the actual setup
    InteractiveSetup();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

//*****************************************************************************/
//    Operation Name: CSetupApp::ExitInstance()
//
//    Processing: 
//
//    Input Parameters:	NONE
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

int CSetupApp::ExitInstance() 
{

    return m_fExitCode;
}

//*****************************************************************************/
//    Operation Name: GetServicePackNumber()
//
//    Processing: 
//
//    Input Parameters:	NONE
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

static int GetServicePackNumber()
{
    const TCHAR* szCSDVersion = GetOSVersionInfo().szCSDVersion;
    const TCHAR szServicePack[] = _T("Service Pack ");
    const int cchServicePack = _tcslen(szServicePack);
    int iNumber = 0;

    if(_tcsnicmp(szCSDVersion, szServicePack, cchServicePack) == 0)
        iNumber = _ttoi(szCSDVersion + cchServicePack);

    return iNumber;
}

//*****************************************************************************/
//    Operation Name: CSetupApp::InteractiveSetup()
//
//    Processing: 
//
//    Input Parameters:	NONE
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

void CSetupApp::InteractiveSetup()
{
    CSetupWindow* psetwnd = NULL;
    CPropertySheet* psht = NULL;
	const int BUFSIZE = 64;
	TCHAR buffer[BUFSIZE];
    try
    {
        //CInitializer::InitializeAll();

        // Check version numbers. 	Require
        // Win32s 1.3, Win95, or WinNT 3.51 as minimums 
        BOOL fOSVersionSupported = FALSE;
        DWORD dwMajor = GetOSVersionInfo().dwMajorVersion;
        DWORD dwMinor = GetOSVersionInfo().dwMinorVersion;
        switch(GetOSVersionInfo().dwPlatformId)
        {
            case VER_PLATFORM_WIN32_WINDOWS:
                fOSVersionSupported = TRUE;
                break;
            case VER_PLATFORM_WIN32_NT:
                if(dwMajor > 3 || (dwMajor == 3 && dwMinor >= 51))
                    fOSVersionSupported = TRUE;
                break;
        }
        if(!fOSVersionSupported)
        {
            UINT idsMessage = IDP_SUPPORTEDOS;
            CString strApp; 
            VERIFY(strApp.LoadString(IDS_INSTALLEDAPPLICATIONTITLE));
            CString strSetupApp; 
            VERIFY(strSetupApp.LoadString(IDS_SETUPAPPLICATIONTITLE));
            CString strMessage; 
            strMessage.Format(idsMessage, 
                (const TCHAR*)strApp, (const TCHAR*)strSetupApp);
			CString message;
			message.Format(IDP_SUPPORTEDOS,(LPCTSTR) strMessage);
 

            AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
            m_fExitCode = EC_FAILURE;
            return;
        }

        CInstaller instlr;


        psetwnd = new CSetupWindow;
        VERIFY(psetwnd->Create());
        m_pMainWnd = psetwnd;
        m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);

        psht = new CSetupSheet(&instlr, m_pMainWnd);

        if(psht)
        {
			//launch the wizard
            if(psht->DoModal() == ID_WIZFINISH )
            {
			    //at this point, a successful connection with the ventilator
				//  has been verified.  The PC is receiving "sync" characters
				//  so a dialog with the ventilator can begin

                //launch the progress dialog
                CSetupProgressDlg progdlg;
                progdlg.Create(m_pMainWnd);

				//center the progress dialog
				RECT parent;
				RECT child;
				psetwnd->GetClientRect(&parent);
				progdlg.GetWindowRect(&child);
				int parentWidth = parent.right - parent.left;
				int parentHeight = parent.bottom - parent.top;
				int childWidth = child.right - child.left;
				int childHeight = child.bottom - child.top;
				progdlg.MoveWindow( (parentWidth - childWidth)/2,
					(parentHeight - childHeight)/2,
					childWidth,
					childHeight);

				// begin the download
				if ( TRUE != m_bSerialNoOnly )
					instlr.Install(&progdlg);

                // the download is complete
                // (all error conditions are caught by exception handlers)

				// check for command lie option tck
				if( m_bFactorySerialNo || m_bAnySerialNo || m_bSerialNoOnly)
				{
					instlr.SetSerialNumber() ;
				}

                m_fExitCode = EC_SUCCESS;

                progdlg.DestroyWindow();

                //create a message dialog to indicate that the installation
				//  was successful
                CString strAppTitle; 
                VERIFY(strAppTitle.LoadString(IDS_INSTALLEDAPPLICATIONTITLE));
                CString strMessage;
                strMessage.Format(IDS_SUCCESS_MESSAGE,(const TCHAR*)strAppTitle);
                CExitSuccess exs(m_pMainWnd);
                exs.m_strMessage = strMessage;
                exs.DoModal();
            }
        }
    }
    catch(CUserException* puex)
    {
        m_fExitCode = EC_CANCELED;

        AfxMessageBox(IDP_SETUPABORTED,MB_OK | MB_ICONEXCLAMATION);
        puex->Delete();
    }
    catch(CException* pex)
    {
        m_fExitCode = EC_FAILURE;
        pex->GetErrorMessage(buffer,BUFSIZE);
        CString strReason(buffer);
        CString strMessage;
        strMessage.Format(IDP_SETUPFAILED, static_cast<const TCHAR*>(strReason));
        AfxMessageBox(strMessage, MB_OK | MB_ICONEXCLAMATION);
        pex->Delete();
    }

    delete psht;
    psht = NULL;

    if(psetwnd)
    {
        m_pMainWnd = NULL;
        psetwnd->DestroyWindow();
        delete psetwnd;
        psetwnd = NULL;
    }
}

