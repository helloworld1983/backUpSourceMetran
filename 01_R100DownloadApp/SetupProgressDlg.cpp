//$COMMON.CPP$
//   File Name:  SetupProgressDlg.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  dialog which records the progress of the download with a
//      bar graph
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
#include "resource.h"
#include "SetupProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProgressDlg dialog

CSetupProgressDlg::CSetupProgressDlg()
{
  m_bCancel=FALSE;
  m_nLower=0;
  m_nUpper=100;
  m_nStep=1;
    m_cbToCopy = 0;
    m_cbCopied = 0;

    // Progress info must be scaled to signed words - 16 bits
    // nScaleFactor = 0x10000 implies that we can copy up to 0x7FFFFFFF (LONG_MAX)
    // bytes and that progress will be reported in 0x10000 (65536) byte increments
    m_nScaleFactor = 0x10000;

  //{{AFX_DATA_INIT(CSetupProgressDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

CSetupProgressDlg::~CSetupProgressDlg()
{
  if(m_hWnd!=NULL)
    DestroyWindow();
}

BOOL CSetupProgressDlg::Create(CWnd *pParent)
{
  return CDialog::Create(CSetupProgressDlg::IDD, pParent);
}

void CSetupProgressDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSetupProgressDlg)
  DDX_Control(pDX, IDC_PROGDLG_PROGRESS, m_Progress);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupProgressDlg, CDialog)
  //{{AFX_MSG_MAP(CSetupProgressDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSetupProgressDlg::SetStatus(LPCTSTR lpszMessage)
{
  ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                  // been created. Can be called from OnInitDialog
  CWnd *pWndStatus = GetDlgItem(IDC_PROGDLG_STATUS);


  // Verify that the static text control exists
  ASSERT(pWndStatus!=NULL);
  pWndStatus->SetWindowText(lpszMessage);

  if(CheckCancelButton())
        AfxThrowUserException();

}

void CSetupProgressDlg::OnCancel()
{
    m_bCancel=TRUE;
}

void CSetupProgressDlg::SetRange(int nLower,int nUpper)
{
  m_nLower = nLower;
  m_nUpper = nUpper;
  m_Progress.SetRange(nLower,nUpper);
}
  
int CSetupProgressDlg::SetPos(int nPos)
{
  PumpMessages();
  int iResult = m_Progress.SetPos(nPos);
  UpdatePercent(nPos);
  return iResult;
}

int CSetupProgressDlg::SetStep(int nStep)
{
  m_nStep = nStep; // Store for later use in calculating percentage
  return m_Progress.SetStep(nStep);
}

int CSetupProgressDlg::OffsetPos(int nPos)
{
  PumpMessages();
  int iResult = m_Progress.OffsetPos(nPos);
  UpdatePercent(iResult+nPos);
  return iResult;
}

int CSetupProgressDlg::StepIt()
{
  PumpMessages();
  int iResult = m_Progress.StepIt();
  UpdatePercent(iResult+m_nStep);
  return iResult;
}

void CSetupProgressDlg::PumpMessages()
{
  // Must call Create() before using the dialog
  ASSERT(m_hWnd!=NULL);

  MSG msg;
  // Handle dialog messages
  while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    if(!IsDialogMessage(&msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);  
    }
  }
}

BOOL CSetupProgressDlg::CheckCancelButton()
{
  // Process all pending messages
  PumpMessages();

  // Reset m_bCancel to FALSE so that
  // CheckCancelButton returns FALSE until the user
  // clicks Cancel again. This will allow you to call
  // CheckCancelButton and still continue the operation.
  // If m_bCancel stayed TRUE, then the next call to
  // CheckCancelButton would always return TRUE

  BOOL bResult = m_bCancel;
  m_bCancel = FALSE;

  return bResult;
}

void CSetupProgressDlg::UpdatePercent(int nNewPos)
{
  CWnd *pWndPercent = GetDlgItem(IDC_PROGDLG_PERCENT);
  int nPercent;
  
  int nDivisor = m_nUpper - m_nLower;
  ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

  int nDividend = (nNewPos - m_nLower);
  ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

  nPercent = nDividend * 100 / nDivisor;

  // Since the Progress Control wraps, we will wrap the percentage
  // along with it. However, don't reset 100% back to 0%
  if(nPercent!=100)
    nPercent %= 100;

  // Display the percentage
  CString strBuf;
  strBuf.Format(_T("%d%c"),nPercent,_T('%'));

    CString strCur; // get current percentage
  pWndPercent->GetWindowText(strCur);

    if (strCur != strBuf)
        pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CSetupProgressDlg message handlers

BOOL CSetupProgressDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  m_Progress.SetRange(m_nLower,m_nUpper);
  m_Progress.SetStep(m_nStep);
  m_Progress.SetPos(m_nLower);

  return TRUE;  
}

void CSetupProgressDlg::SetCountBytesToCopy(long cbToCopy)
{
    m_nScaleFactor = 1 + cbToCopy/SHRT_MAX;
    m_cbToCopy = cbToCopy;
    m_cbCopied = 0;

    ASSERT(m_cbToCopy/m_nScaleFactor <= SHRT_MAX);

    TRACE(_T("INFO: %ld bytes to copy with scale factor %ld and range 0 to %d\n"),
        m_cbToCopy, m_nScaleFactor, static_cast<int>(m_cbToCopy/m_nScaleFactor));

    SetRange(0, static_cast<int>(m_cbToCopy/m_nScaleFactor));
}

void CSetupProgressDlg::SetStatusMessage(const TCHAR* szStatus)
{
    SetStatus(szStatus);
}

void CSetupProgressDlg::ReportProgress(long cbJustCopied)
{
    m_cbCopied += cbJustCopied;
    ASSERT(m_cbCopied/m_nScaleFactor <= SHRT_MAX);
    SetPos(static_cast<int>(m_cbCopied/m_nScaleFactor));
    if(CheckCancelButton())
        AfxThrowUserException();
}

void CSetupProgressDlg::SetProgress(long cbProgress)
{
    m_cbCopied = cbProgress;
    ASSERT(m_cbCopied/m_nScaleFactor <= SHRT_MAX);
    SetPos(static_cast<int>(m_cbCopied/m_nScaleFactor));
}

long CSetupProgressDlg::GetProgress()
{
    return m_cbCopied;
}



