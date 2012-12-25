#include "StdAfx.h"
#include "MessagesDlg.h"
#include "microsip.h"
#include "microsipDlg.h"
#include "settings.h"
#include "Transfer.h"

static CmicrosipDlg *microsipDlg;

DWORD __stdcall MEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString sThisWrite;
	sThisWrite.GetBufferSetLength(cb);

	CString *psBuffer = (CString *)dwCookie;

	for (int i=0;i<cb;i++)
	{
		sThisWrite.SetAt(i,*(pbBuff+i));
	}

	*psBuffer += sThisWrite;

	*pcb = sThisWrite.GetLength();
	sThisWrite.ReleaseBuffer();
	return 0;
}

MessagesDlg::MessagesDlg(CWnd* pParent /*=NULL*/)
: CDialog(MessagesDlg::IDD, pParent)
{
	this->m_hWnd = NULL;
	microsipDlg = (CmicrosipDlg* )pParent;
	Create (IDD, pParent);
}

MessagesDlg::~MessagesDlg(void)
{
}

void MessagesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, tabComponent);
}

BOOL MessagesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	tab = &tabComponent;

	TranslateDialog(this->m_hWnd);

#ifndef _GLOBAL_VIDEO
	GetDlgItem(IDC_VIDEO_CALL)->ShowWindow(SW_HIDE);
#endif

	CRichEditCtrl* richEdit = (CRichEditCtrl*)GetDlgItem(IDC_MESSAGE);
	richEdit->SetEventMask(richEdit->GetEventMask() | ENM_KEYEVENTS);

	CRichEditCtrl* richEditList = (CRichEditCtrl*)GetDlgItem(IDC_LIST);

	CFont* font = this->GetFont();
	LOGFONT lf;
	font->GetLogFont(&lf);
	lf.lfHeight = 16;
	fontList.CreateFontIndirect(&lf);
	richEditList->SetFont(&fontList);
	lf.lfHeight = 18;
	fontMessage.CreateFontIndirect(&lf);
	richEdit->SetFont(&fontMessage);

	para.cbSize=sizeof(PARAFORMAT2);
	para.dwMask= PFM_STARTINDENT | PFM_SPACEBEFORE;
	para.dxStartIndent=100;
	para.dySpaceBefore=100;
	richEditList->SetParaFormat(para);

	return TRUE;
}


void MessagesDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(MessagesDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &MessagesDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &MessagesDlg::OnBnClickedOk)
	ON_NOTIFY(EN_MSGFILTER, IDC_MESSAGE, &MessagesDlg::OnEnMsgfilterMessage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &MessagesDlg::OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, &MessagesDlg::OnTcnSelchangingTab)
	ON_MESSAGE(UM_CLOSETAB, &MessagesDlg::OnCloseTab)
	ON_BN_CLICKED(IDC_CALL_END, &MessagesDlg::OnBnClickedCallEnd)
	ON_BN_CLICKED(IDC_VIDEO_CALL, &MessagesDlg::OnBnClickedVideoCall)
	ON_BN_CLICKED(IDC_HOLD, &MessagesDlg::OnBnClickedHold)
	ON_BN_CLICKED(IDC_TRANSFER, &MessagesDlg::OnBnClickedTransfer)
	ON_BN_CLICKED(IDC_END, &MessagesDlg::OnBnClickedEnd)
END_MESSAGE_MAP()

void MessagesDlg::OnClose() 
{
	pjsua_call_hangup_all();
	this->ShowWindow(SW_HIDE);
}

void MessagesDlg::OnBnClickedCancel()
{
	OnClose();
}

void MessagesDlg::OnBnClickedOk()
{
}

MessagesContact* MessagesDlg::AddTab(CString number, CString name, BOOL activate, pjsua_call_info *call_info, BOOL notShowWindow, BOOL ifExists)
{
	MessagesContact* messagesContact;

	SIPURI sipuri;
	ParseSIPURI(number, &sipuri);
	number = (sipuri.user.GetLength() ? sipuri.user + "@" : "") + sipuri.domain;

	LONG exists = -1;
	for (int i=0; i < tab->GetItemCount(); i++)
	{
		messagesContact = GetMessageContact(i);
		if (messagesContact->number == number)
		{
			if (call_info)
			{
				if (messagesContact->callId != -1 && messagesContact->callId != call_info->id)
				{
					pjsua_call_hangup(messagesContact->callId, 0, NULL, NULL);
				}
				messagesContact->callId = call_info->id;
			}
			exists=i;
			break;
		}
	}
	if (exists==-1)
	{
		if (ifExists)
		{
			return NULL;
		}
		if (!name.GetLength()) {
			name = microsipDlg->pageContacts->GetNameByNumber(number);
			if (!name.GetLength()) {
				if (!sipuri.name.GetLength())
				{
					name = (sipuri.domain == accountSettings.domain ? sipuri.user : number);
				} else 
				{
					name = sipuri.name + " <" + number + ">";
				}
			}
		}
		messagesContact = new MessagesContact();
		messagesContact->callId = call_info ? call_info->id : -1;
		messagesContact->number = number;
		messagesContact->name = name;
		TCITEMA item;
		item.mask = TCIF_PARAM | TCIF_TEXT;
		name.Format("   %s  ", name);
		item.pszText=name.GetBuffer();
		item.cchTextMax=0;
		item.lParam = (LPARAM)messagesContact;
		exists = tab->InsertItem(tab->GetItemCount(),&item);
		if (tab->GetCurSel() == exists)
		{
			OnChangeTab(call_info);
		}
	} else
	{
		/*
		if (tab->GetCurSel() == exists)
		{
			UpdateCallButton(messagesContact->callId != -1, call_info);
		}
		*/
	}
	if (tab->GetCurSel() != exists && (activate || !IsWindowVisible()))
	{
		LRESULT pResult;
		OnTcnSelchangingTab(NULL, &pResult);
		tab->SetCurSel(exists);
		OnChangeTab(call_info);
	}
	if (!IsWindowVisible()) {
		if (!notShowWindow) 
		{
			if (!accountSettings.hidden) {
				ShowWindow(SW_SHOW);
			}
		}
	} else {
		SetForegroundWindow();
	}
	return messagesContact;
}

void MessagesDlg::OnChangeTab(pjsua_call_info *p_call_info)
{
	tab->HighlightItem(tab->GetCurSel(),FALSE);

	MessagesContact* messagesContact = GetMessageContact();
	SetWindowText(" " + messagesContact->name);
		
	if (messagesContact->callId != -1) {
		if (p_call_info) {
			UpdateCallButton(TRUE, p_call_info);
		} else {
			pjsua_call_info call_info;
			pjsua_call_get_info(messagesContact->callId, &call_info);
			UpdateCallButton(TRUE, &call_info);
		}
	} else {
		UpdateCallButton();
	}

	CRichEditCtrl* richEditList = (CRichEditCtrl*)GetDlgItem(IDC_LIST);
	CRichEditCtrl* richEdit = (CRichEditCtrl*)GetDlgItem(IDC_MESSAGE);

	richEditList->SetWindowText(messagesContact->messages);
	richEditList->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	richEdit->SetWindowText(messagesContact->message);

	if (!accountSettings.singleMode)
	{
		GotoDlgCtrl(richEdit);
	}

	int nEnd = richEdit->GetTextLengthEx(GTL_NUMCHARS);
	richEdit->SetSel(nEnd, nEnd);
}

void MessagesDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnChangeTab();
	*pResult = 0;
}


void MessagesDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	CRichEditCtrl* richEdit = (CRichEditCtrl*)GetDlgItem(IDC_MESSAGE);
	CString str;
	int len = richEdit->GetWindowTextLength();
	char *ptr = str.GetBuffer(len);
	richEdit->GetWindowText(ptr,len+1);
	str.ReleaseBuffer();

	MessagesContact* messagesContact = GetMessageContact();
	messagesContact->message = str;
	*pResult = 0;
}

LRESULT  MessagesDlg::OnCloseTab(WPARAM wParam,LPARAM lParam)
{
	int i=wParam;
	int curSel = tab->GetCurSel();

	MessagesContact* messagesContact = GetMessageContact(i);
	if (messagesContact->callId != -1)
	{
		pjsua_call_hangup(messagesContact->callId, 0, NULL, NULL);
	}
	delete messagesContact;
	tab->DeleteItem(i);
	int count = tab->GetItemCount();
	if (!count) {
		GetDlgItem(IDC_MESSAGE)->SetWindowText(NULL);
		GetDlgItem(IDC_LIST)->SetWindowText(NULL);
		OnClose();
	} else  {
		tab->SetCurSel( curSel < count ? curSel: count-1 );
		OnChangeTab();
	}

	return TRUE;
}


void MessagesDlg::CallStart(BOOL hasVideo)
{
	MessagesContact* messagesContact = GetMessageContact();
	pj_str_t pj_uri = pj_str ( StrToPj ( microsipDlg->GetSIPURI(messagesContact->number) ) );
	pjsua_call_id call_id;
	
	pjsua_call_setting call_setting;
	pjsua_call_setting_default(&call_setting);
	call_setting.flag = 0;
	call_setting.vid_cnt=hasVideo ? 1 : 0;
		
	pj_status_t status = pjsua_call_make_call(
		pjsua_acc_is_valid(microsipDlg->account) ? microsipDlg->account : NULL,
		&pj_uri,
		&call_setting,
		NULL,
		NULL,
		&call_id);
	if (status != PJ_SUCCESS)
	{
		AddMessage(messagesContact,microsipDlg->GetErrorMessage(status));
	} else {
		messagesContact->callId = call_id;
		UpdateCallButton(TRUE);
	}
}

void MessagesDlg::OnBnClickedCallEnd()
{
	MessagesContact* messagesContact = GetMessageContact();
	if (messagesContact->callId == -1)
	{
		CallStart();
	}
}

void MessagesDlg::OnEndCall(pjsua_call_info *call_info)
{
	for (int i = 0; i < tab->GetItemCount(); i++)
	{
		MessagesContact* messagesContact = GetMessageContact(i);
		if (messagesContact->callId == call_info->id)
		{
			messagesContact->callId = -1;
			if (tab->GetCurSel()==i)
			{
				UpdateCallButton(FALSE, call_info);
			}
			break;
		}
	}
}

void MessagesDlg::UpdateCallButton(BOOL active, pjsua_call_info *call_info)
{
	GetDlgItem(IDC_CALL_END)->ShowWindow(active? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_END)->ShowWindow(!active? SW_HIDE : SW_SHOW);
#ifdef _GLOBAL_VIDEO
	GetDlgItem(IDC_VIDEO_CALL)->ShowWindow(active? SW_HIDE : SW_SHOW);
#endif
	UpdateHoldButton(call_info);
}

void MessagesDlg::UpdateHoldButton(pjsua_call_info *call_info)
{
#ifndef _GLOBAL_NO_HOLD
	MessagesContact* messagesContact = GetMessageContact();
	if (messagesContact) {
		CButton* button = (CButton*)GetDlgItem(IDC_HOLD);
		CButton* buttonTransfer = (CButton*)GetDlgItem(IDC_TRANSFER);
		if (messagesContact->callId != -1 && call_info && messagesContact->callId == call_info->id) {
			if (call_info->media_status == PJSUA_CALL_MEDIA_ACTIVE) {
				button->SetWindowText(Translate("Hold"));
				button->ShowWindow(SW_SHOW);
				buttonTransfer->ShowWindow(SW_SHOW);
				return;
			}
			if (call_info->media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD ) {
				button->SetWindowText(Translate("Release"));
				button->ShowWindow(SW_SHOW);
				buttonTransfer->ShowWindow(SW_HIDE);
				return;
			}
		}
		button->ShowWindow(SW_HIDE);
		buttonTransfer->ShowWindow(SW_HIDE);
	}
#endif
}

void MessagesDlg::Call()
{
	if (!accountSettings.singleMode || !pjsua_call_get_count())
	{
		MessagesContact* messagesContact = GetMessageContact();
		if (messagesContact->callId == -1)
		{
			OnBnClickedCallEnd();
		}
	}
}

void MessagesDlg::AddMessage(MessagesContact* messagesContact, CString message, int type)
{
	CRichEditCtrl richEdit;
	MessagesContact* messagesContactSelected = GetMessageContact();
	
	if (type != MSIP_MESSAGE_TYPE_SYSTEM || messagesContact->prevMessage != message)
	{
		CRichEditCtrl *richEditList = (CRichEditCtrl *)GetDlgItem(IDC_LIST);
		if (messagesContactSelected != messagesContact)
		{
			CRect rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = 300;
			rect.bottom = 300;
			richEdit.Create( ES_MULTILINE | ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER | WS_VSCROLL, rect, this, NULL);
			richEdit.SetFont(&fontList);
			richEdit.SetParaFormat(para);	
			richEdit.SetWindowText(messagesContact->messages);
			richEditList = &richEdit;
		}

		CString name;
		if (type==MSIP_MESSAGE_TYPE_LOCAL)
		{
			if (accountSettings.displayName.GetLength())
			{
				name = accountSettings.displayName;
			}
		} else if (type==MSIP_MESSAGE_TYPE_REMOTE)
		{
			name = messagesContact->name;
		}

		int nBegin;
		CHARFORMAT cf;

		CTime time = CTime::GetCurrentTime();
		
		nBegin = richEditList->GetTextLengthEx(GTL_NUMCHARS);
		richEditList->SetSel(nBegin, nBegin);
		richEditList->ReplaceSel( time.Format("%X  ") );
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_SIZE;
		cf.crTextColor = RGB (160,160,160);
		cf.dwEffects = 0;
		cf.yHeight = 150;
		richEditList->SetSel(nBegin,-1);
		richEditList->SetSelectionCharFormat(cf);

		if (name.GetLength())
		{
			nBegin = richEditList->GetTextLengthEx(GTL_NUMCHARS);
			richEditList->SetSel(nBegin, nBegin);
			richEditList->ReplaceSel( name + ": ");
			cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_SIZE;
			cf.crTextColor = RGB (0,0,0);
			cf.dwEffects = CFE_BOLD;
			cf.yHeight = 200;
			richEditList->SetSel(nBegin,-1);
			richEditList->SetSelectionCharFormat(cf);
		}

		nBegin = richEditList->GetTextLengthEx(GTL_NUMCHARS);
		richEditList->SetSel(nBegin, nBegin);
		richEditList->ReplaceSel(message+"\r\n");
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_SIZE;
		
		cf.crTextColor = type == MSIP_MESSAGE_TYPE_SYSTEM ? RGB (160, 160, 160) : RGB (0,0,0);
		cf.dwEffects = 0;
		cf.yHeight = 200;
		richEditList->SetSel(nBegin,-1);
		richEditList->SetSelectionCharFormat(cf);

		if (messagesContactSelected == messagesContact)
		{
			richEditList->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		} else
		{
			for (int i = 0; i < tab->GetItemCount(); i++)
			{
				if (messagesContact == GetMessageContact(i))
				{
					tab->HighlightItem(i, TRUE);
					break;
				}
			}
		}

		CString str;
		EDITSTREAM es;
		es.dwCookie = (DWORD) &str;
		es.pfnCallback = MEditStreamOutCallback; 
		richEditList->StreamOut(SF_RTF, es);
		messagesContact->messages=str;

		messagesContact->prevMessage=message;
	}
}

void MessagesDlg::OnEnMsgfilterMessage(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);

	if (pMsgFilter->msg == WM_CHAR)
	{
		if ( pMsgFilter->wParam == VK_RETURN )
		{

			CRichEditCtrl* richEdit = (CRichEditCtrl*)GetDlgItem(IDC_MESSAGE);
			CString message;
			int len = richEdit->GetWindowTextLength();
			char *ptr = message.GetBuffer(len);
			richEdit->GetWindowText(ptr,len+1);
			message.ReleaseBuffer();
			message.Trim();
			if (message.GetLength())
			{
				MessagesContact* messagesContact = GetMessageContact();

				pj_str_t pj_uri = pj_str ( StrToPj ( microsipDlg->GetSIPURI(messagesContact->number) ) );
				
				pj_str_t pj_message = pj_str ( StrToPj ( message ) );

				pj_status_t status = pjsua_im_send( pjsua_acc_is_valid(microsipDlg->account) ? microsipDlg->account : NULL, &pj_uri, NULL, &pj_message, NULL, NULL );

				if ( status == PJ_SUCCESS )
				{
					richEdit->SetWindowText(NULL);
					GotoDlgCtrl(richEdit);
					AddMessage(messagesContact, message, MSIP_MESSAGE_TYPE_LOCAL);
				}
			}
			*pResult= 1;
			return;
		}
	}
	*pResult = 0;
}

MessagesContact* MessagesDlg::GetMessageContact(int i)
{
	if (i ==-1) {
		i = tab->GetCurSel();
	}
	if (i != -1) {
		TCITEMA item;
		item.mask = TCIF_PARAM;
		tab->GetItem(i, &item);
		return (MessagesContact*) item.lParam;
	}
	return NULL;
}
void MessagesDlg::OnBnClickedVideoCall()
{
	CallStart(TRUE);
}

void MessagesDlg::OnBnClickedHold()
{
	MessagesContact* messagesContactSelected = GetMessageContact();
	if (messagesContactSelected->callId!=-1) {
		pjsua_call_info info;
		pjsua_call_get_info(messagesContactSelected->callId,&info);
		if (info.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD) {
			pjsua_call_reinvite(messagesContactSelected->callId, PJSUA_CALL_UNHOLD, NULL);
		} else {
			pjsua_call_set_hold(messagesContactSelected->callId, NULL);
		}
	}
}

void MessagesDlg::OnBnClickedTransfer()
{
	if (!microsipDlg->transferDlg)
	{
		microsipDlg->transferDlg = new Transfer(this);
	}
	microsipDlg->transferDlg->SetForegroundWindow();
}

void MessagesDlg::OnBnClickedEnd()
{
	MessagesContact* messagesContact = GetMessageContact();
	pjsua_call_hangup(messagesContact->callId, 0, NULL, NULL);
}
