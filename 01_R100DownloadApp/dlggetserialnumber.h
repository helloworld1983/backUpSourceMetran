//    Class:  BootFile
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  04/28/99 	T. Kadien				Morph
//      Description: Initial Revision
//
/******************************************************************************/
#if !defined(AFX_DLGGETSERIALNUMBER_H__B8C59464_FA36_11D2_87A0_444553540000__INCLUDED_)
#define AFX_DLGGETSERIALNUMBER_H__B8C59464_FA36_11D2_87A0_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgGetSerialNumber.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgGetSerialNumber dialog

class CdlgGetSerialNumber : public CDialog
{
// Construction
public:
	CdlgGetSerialNumber(CWnd* pParent = NULL);   // standard constructor
	CString GetSerialNo() {return m_strSerialNumber ;}
// Dialog Data
	//{{AFX_DATA(CdlgGetSerialNumber)
	enum { IDD = IDD_GET_SERIAL_NUMBER };
	CString	m_strSerialNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgGetSerialNumber)
	public:
	virtual int DoModal(BOOL UseMfgSn = TRUE, const CString &strCurrentNumber ="");
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgGetSerialNumber)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_UseMfgSn ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETSERIALNUMBER_H__B8C59464_FA36_11D2_87A0_444553540000__INCLUDED_)
