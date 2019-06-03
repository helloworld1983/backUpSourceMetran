//$COMMON.CPP$
//   File Name:  SetupWindow.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: provides a clean background for the Setup application
//
//   Interfaces:
//
//   Restrictions:
//
//   Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/06/98  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/


#include "stdafx.h"
#include "_Setup.h"
#include "SetupWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupWindow

CSetupWindow::CSetupWindow()
{
}

CSetupWindow::~CSetupWindow()
{
}

BOOL CSetupWindow::Create()
{
    LPCTSTR szClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE, 
        0, HBRUSH(COLOR_APPWORKSPACE + 1), AfxGetApp()->LoadIcon(IDR_MAINFRAME));

    CString strTitle; VERIFY(strTitle.LoadString(IDS_SETUPAPPLICATIONTITLE));
    
    return CreateEx(0, szClass, strTitle, WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL);
}


BEGIN_MESSAGE_MAP(CSetupWindow, CWnd)
    //{{AFX_MSG_MAP(CSetupWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



