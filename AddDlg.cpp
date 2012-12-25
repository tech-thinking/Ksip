#include "stdafx.h"
#include "microsip.h"
#include "AddDlg.h"

#include "microsipDlg.h"

static CmicrosipDlg *microsipDlg;

AddDlg::AddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddDlg::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();	
	Create (IDD, pParent);
}

AddDlg::~AddDlg()
{
}

BOOL AddDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TranslateDialog(this->m_hWnd);
	return TRUE;
}

void AddDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}


BEGIN_MESSAGE_MAP(AddDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &AddDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &AddDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void AddDlg::OnClose() 
{
	this->ShowWindow(SW_HIDE);
}

void AddDlg::OnBnClickedOk()
{
	CString number;
	CString name;
	BOOL presence;
			
	GetDlgItem(IDC_EDIT_NUMBER)->GetWindowText(number);
	number=number.Trim();
	if (number.GetLength()) {
		GetDlgItem(IDC_EDIT_NAME)->GetWindowText(name);
		name=name.Trim();
		name=name.GetLength()?name:number;
		if (listIndex != -1) {
			microsipDlg->pageContacts->ContactDelete(listIndex);
		}
#ifndef _GLOBAL_ACCOUNT_MINI
		presence = ((CButton*)GetDlgItem(IDC_PRESENCE))->GetCheck();
#else
		presence = 0;
#endif
		microsipDlg->pageContacts->ContactAdd(number, name, presence, TRUE);

		OnClose();
	}
}

void AddDlg::OnBnClickedCancel()
{
	OnClose();
}
