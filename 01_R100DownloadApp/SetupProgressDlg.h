//$COMMON.H$
//    File Name:  SetupProgressDlg.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupProgressDlg
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	blumee				Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  SETUPPROGRESSDLG_H
#define	 SETUPPROGRESSDLG_H


class CSetupProgressDlg 
	: public CDialog  //, public CCopyProgress
{
// Construction / Destruction
public:
  CSetupProgressDlg();   // standard constructor
  ~CSetupProgressDlg();

  BOOL Create(CWnd *pParent=NULL);

public:
	long m_nScaleFactor;

  // Checking for Cancel button
  BOOL CheckCancelButton();
  // Progress Dialog manipulation
  void SetStatus(LPCTSTR lpszMessage);

// CCopyProgress interface
	virtual void SetCountBytesToCopy(long cbToCopy);
	virtual void SetStatusMessage(const TCHAR* szStatus);
	virtual void ReportProgress(long cbJustCopied);
	virtual void SetProgress(long cbCopied);
	virtual long GetProgress();

// Dialog Data
  //{{AFX_DATA(CSetupProgressDlg)
  enum { IDD = IDD_PROGRESS };
  CProgressCtrl	m_Progress;
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSetupProgressDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nCaptionID;
	int m_nLower;
	int m_nUpper;
	int m_nStep;

	BOOL m_bCancel;

	long m_cbToCopy;
	long m_cbCopied;

	void SetRange(int nLower,int nUpper);
	int  SetStep(int nStep);
	int  SetPos(int nPos);
	int  OffsetPos(int nPos);
	int  StepIt();
	void UpdatePercent(int nCurrent);
	void PumpMessages();
	virtual void OnCancel();
	virtual void OnOK() {}; 

	// Generated message map functions
	//{{AFX_MSG(CSetupProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__
