#include "StdAfx.h"
#include "RinginDlg.h"
#include "langpack.h"

RinginDlg::~RinginDlg(void)
{
}

RinginDlg::RinginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RinginDlg::IDD, pParent)
{
	Create (IDD, pParent);
}

void RinginDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BOOL RinginDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	TranslateDialog(this->m_hWnd);

	CFont* font = this->GetFont();
	LOGFONT lf;
	font->GetLogFont(&lf);
	lf.lfHeight = 24;
	lf.lfWeight = 1000;
	m_font.CreateFontIndirect(&lf);
	
	GetDlgItem(IDC_CALLER_NAME)->SetFont(&m_font);

	return TRUE;
}

BEGIN_MESSAGE_MAP(RinginDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &RinginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RinginDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_VIDEO, &RinginDlg::OnBnClickedVideo)
END_MESSAGE_MAP()

void RinginDlg::OnClose() 
{
	pjsua_call_hangup(call_id, 0, NULL, NULL);
	this->ShowWindow(SW_HIDE);
}

void RinginDlg::OnBnClickedOk()
{
	CallAccept();
}

void RinginDlg::OnBnClickedCancel()
{
	OnClose();
}

void RinginDlg::OnBnClickedVideo()
{
	CallAccept(TRUE);
}

void RinginDlg::CallAccept(BOOL hasVideo)
{
	pjsua_call_setting call_setting;
	pjsua_call_setting_default(&call_setting);
	call_setting.vid_cnt=hasVideo ? 1:0;
	call_setting.flag = 0;
	pjsua_call_answer2(call_id, &call_setting, 200, NULL, NULL);
	this->ShowWindow(SW_HIDE);
}