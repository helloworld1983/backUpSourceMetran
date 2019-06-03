//$COMMON.CPP$
//   File Name:  ExitSuccess.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: a modal dialog which informs the user that the download
//      operation was succesful
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
#include "ExitSuccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExitSuccess dialog


CExitSuccess::CExitSuccess(CWnd* pParent /*=NULL*/)
    : CDialog(CExitSuccess::IDD, pParent)
{
    //{{AFX_DATA_INIT(CExitSuccess)
    m_strMessage = _T("");
    //}}AFX_DATA_INIT
}


void CExitSuccess::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CExitSuccess)
    DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExitSuccess, CDialog)
    //{{AFX_MSG_MAP(CExitSuccess)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExitSuccess message handlers


BOOL CExitSuccess::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CString strSetupTitle; 
    VERIFY(strSetupTitle.LoadString(IDS_SETUPAPPLICATIONTITLE));
    CString strFormat;
    CString strSucceeded;

    GetDlgItemText(IDC_SUCCEEDED, strFormat);
    strSucceeded.Format(strFormat, strSetupTitle);
    SetDlgItemText(IDC_SUCCEEDED, strSucceeded);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

