//$COMMON.H$
//    File Name:  ExitSuccess.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CExitSuccess
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	Erik Blume			Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  EXITSUCCESS_H
#define	 EXITSUCCESS_H


/////////////////////////////////////////////////////////////////////////////
// CExitSuccess dialog

class CExitSuccess : public CDialog
{
// Construction
public:
	CExitSuccess(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExitSuccess)
	enum { IDD = IDD_EXITSUCCESS };
	CString	m_strMessage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExitSuccess)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExitSuccess)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif