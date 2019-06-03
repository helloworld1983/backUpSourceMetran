// dlgGetSerialNumber.cpp : implementation file
//

#include "stdafx.h"
#include "_setup.h"
#include "dlgGetSerialNumber.h"
#include "downloadprotocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *DEFAULT_SN_ID = "VS";
/////////////////////////////////////////////////////////////////////////////
// CdlgGetSerialNumber dialog


CdlgGetSerialNumber::CdlgGetSerialNumber(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgGetSerialNumber::IDD, pParent),
	m_UseMfgSn ( TRUE )
{
	//{{AFX_DATA_INIT(CdlgGetSerialNumber)
	m_strSerialNumber = _T("");
	//}}AFX_DATA_INIT
}


void CdlgGetSerialNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgGetSerialNumber)
	DDX_Text(pDX, IDC_SERIAL_NUMBER, m_strSerialNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgGetSerialNumber, CDialog)
	//{{AFX_MSG_MAP(CdlgGetSerialNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgGetSerialNumber message handlers

BOOL CdlgGetSerialNumber::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GotoDlgCtrl( GetDlgItem ( IDC_SERIAL_NUMBER ) );

	if (FALSE == m_UseMfgSn)
	{
		CWnd* pApplyButton = GetDlgItem (IDC_SN_NOTE);	// Get the control
		ASSERT (pApplyButton);							// check control id
		
		pApplyButton->ShowWindow (FALSE);				// hide the control
	}

	VERIFY (UpdateData(FALSE) != FALSE );						// update field in dialog box

	return FALSE;  // return TRUE unless you set the focus to a control
}

void CdlgGetSerialNumber::OnOK() 
{
	VERIFY (UpdateData(TRUE) != FALSE ) ;			// Retrieve information from dialog box

	m_strSerialNumber.MakeUpper() ;
	if (m_strSerialNumber.GetLength() > 0 )
	{
		m_strSerialNumber+= CString(" ", SERIAL_NUMBER_SIZE) ;
		m_strSerialNumber =m_strSerialNumber.Left(SERIAL_NUMBER_SIZE) ;
	}

	VERIFY (UpdateData(FALSE) != FALSE ) ;			// Retrieve information from dialog box

	// If manufacturing serial number flag set
	// close dialog box only if it begins with VS
	if ( m_UseMfgSn )
		if ( 0 != strncmp (m_strSerialNumber, DEFAULT_SN_ID, strlen(DEFAULT_SN_ID) ) )
		{
			AfxMessageBox("Serial number must start with VS", IDOK);
			return ;
		}

	CDialog::OnOK();
}

int CdlgGetSerialNumber::DoModal(BOOL UseMfgSn, const CString &strCurrentNumber ) 
{
	m_UseMfgSn = UseMfgSn ;
	m_strSerialNumber = strCurrentNumber.Left(6) ;
	
	return CDialog::DoModal();
}
