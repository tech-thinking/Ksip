#include "StdAfx.h"
#include "Transfer.h"
#include "microsipDlg.h"

static CmicrosipDlg *microsipDlg;

Transfer::Transfer(CWnd* pParent /*=NULL*/)
: CDialog(Transfer::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();	
	Create (IDD, pParent);
}

Transfer::~Transfer(void)
{
	microsipDlg->transferDlg = NULL;
}


BOOL Transfer::OnInitDialog()
{
	CDialog::OnInitDialog();
	TranslateDialog(this->m_hWnd);

	return TRUE;
}

void Transfer::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(Transfer, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &Transfer::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &Transfer::OnBnClickedOk)
END_MESSAGE_MAP()


void Transfer::OnClose() 
{
	DestroyWindow();
}

void Transfer::OnBnClickedCancel()
{
	OnClose();
}

void Transfer::OnBnClickedOk()
{
	MessagesContact* messagesContactSelected = microsipDlg->messagesDlg->GetMessageContact();
	if (messagesContactSelected->callId!=-1) {
		CString number;
		GetDlgItem(IDC_NUMBER)->GetWindowText(number);
		number.Trim();
		if (number != "") {
			pj_str_t pj_uri = pj_str ( StrToPj ( microsipDlg->GetSIPURI(number, TRUE) ) );
			pjsua_call_xfer(messagesContactSelected->callId, &pj_uri, NULL);
			OnClose();
		}
	}
}
