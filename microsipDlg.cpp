#include "stdafx.h"
#include "microsip.h"
#include "microsipDlg.h"
#include "Mmsystem.h"
#include "settings.h"
#include "global.h"
#include "ModelessMessageBox.h"

#include <io.h>
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CmicrosipDlg *microsipDlg;

static UINT BASED_CODE indicators[] =
{
	ID_STATUSBAR
};

static void on_reg_state2(pjsua_acc_id acc_id,  pjsua_reg_info *info)
{
	if (IsWindow(microsipDlg->m_hWnd))
	{
#ifdef _GLOBAL_BALANCE
		pjsip_generic_string_hdr *hsr = NULL;
		const pj_str_t name = {"X-DSSBalance",12};
		hsr = (pjsip_generic_string_hdr*) pjsip_msg_find_hdr_by_name ( info->cbparam->rdata->msg_info.msg, &name, NULL);
		if (hsr) {
			microsipDlg->balance = PjToStr(&hsr->hvalue);
		} else {
			microsipDlg->balance = "";
		}
#endif
		microsipDlg->PostMessage(MYWM_UPDATE_WINDOW_TEXT);
	}
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	if (IsWindow(microsipDlg->m_hWnd))
	{
		pjsua_call_info call_info;
		pjsua_call_get_info(call_id, &call_info);
		microsipDlg->SendMessage(MYWM_ON_CALL_STATE, (WPARAM) &call_info, 0);
	}
}

static void on_call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info call_info;
	pjsua_call_get_info(call_id, &call_info);

	microsipDlg->messagesDlg->UpdateHoldButton(&call_info);

	if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE)
	{
		pjsua_conf_connect(call_info.conf_slot, 0);
		pjsua_conf_connect(0, call_info.conf_slot);
	} else {
		call_deinit_tonegen(call_id);
		pjsua_conf_disconnect(call_info.conf_slot, 0);
		pjsua_conf_disconnect(0, call_info.conf_slot);
	}
#ifdef _GLOBAL_VIDEO
	if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE)	{
		for (int i=0;i<call_info.media_cnt;i++) {
			if (call_info.media[i].type==PJMEDIA_TYPE_VIDEO
				&&
				(
				call_info.media[i].dir==PJMEDIA_DIR_ENCODING 
				||
				call_info.media[i].dir==PJMEDIA_DIR_ENCODING_DECODING
				)
				) {
					microsipDlg->PostMessage(MYWM_CREATE_PREVIEW);
					break;
			}
		}
	}
#endif
}

static void on_incoming_call(pjsua_acc_id acc, pjsua_call_id call_id,
							 pjsip_rx_data *rdata)
{
	if (IsWindow(microsipDlg->m_hWnd))
	{
		pjsua_call_info call_info;
		pjsua_call_get_info(call_id,&call_info);
		if (!accountSettings.autoAnswer)
		{
			if (accountSettings.hidden) {
				pjsua_call_hangup(call_id, 0, NULL, NULL);
			} else {
				microsipDlg->SendMessage(MYWM_CREATE_RINGING, (WPARAM)&call_info, NULL);
				pjsua_call_answer(call_id,180,NULL,NULL);

				PlaySound(NULL, 0, 0);
				if (!accountSettings.ringingSound.GetLength()) {
					PlaySound(MAKEINTRESOURCE(IDR_WAVE_RINGIN), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
				} else
				{
					PlaySound(accountSettings.ringingSound, GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC | SND_LOOP);
				}
			}
		} else
		{
			microsipDlg->PostMessage(MYWM_CALL_ANSWER, (WPARAM)call_id, (LPARAM)call_info.rem_vid_cnt);
			if (!accountSettings.hidden) {
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_RINGIN2), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
			}
		}
	}
}

static void on_nat_detect(const pj_stun_nat_detect_result *res)
{
	if (res->status != PJ_SUCCESS)
	{
		pjsua_perror(THIS_FILE, "NAT detection failed", res->status);
	} else
	{
		if (res->nat_type == PJ_STUN_NAT_TYPE_SYMMETRIC)
		{
			if (IsWindow(microsipDlg->m_hWnd))
			{
				CString message = "STUN technology not works with Symmetric NAT. To avoid media problems, remove STUN server from account config.";
#ifndef _GLOBAL_CUSTOM
				message.Append(" For more info visit MicroSIP website, help page.");
#endif
				microsipDlg->BaloonPopup("Symmetric NAT detected!", message);
			}
		}
		PJ_LOG(3, (THIS_FILE, "NAT detected as %s", res->nat_type_name));
	}
}

static void on_buddy_state(pjsua_buddy_id buddy_id)
{
	int image;
	pjsua_buddy_info info;
	pjsua_buddy_get_info (buddy_id, &info);

	switch (info.status)
	{
	case PJSUA_BUDDY_STATUS_OFFLINE:
		image=1;
		break;
	case PJSUA_BUDDY_STATUS_ONLINE:
		if (info.rpid.activity == PJRPID_ACTIVITY_AWAY || info.rpid.activity == PJRPID_ACTIVITY_BUSY)
		{
			image=2;
		} else 
		{
			image=3;
		}
		break;
	default:
		image=0;
		break;
	}
	if (IsWindow(microsipDlg->m_hWnd))
	{
		CString usersOnline;
		CTime t = CTime::GetCurrentTime();
		time_t time = t.GetTime();
		CListCtrl *list= (CListCtrl*)microsipDlg->pageContacts->GetDlgItem(IDC_CONTACTS);
		int n = list->GetItemCount();
		for (int i=0; i<n; i++) {
			Contact *contact = (Contact *)list->GetItemData(i);
			if (contact==(Contact *)pjsua_buddy_get_user_data(buddy_id)) {
				//list->GetItemData(
				list->SetItem(i, 0, LVIF_IMAGE, 0, image, 0, 0, 0);
				//if (1 || image==2 || image==3) 
				//{
				//	contact->presenceTime = time;
				//}
				//break;
			}
			/*
			if (contact->presenceTime > time - 10)
			{
			usersOnline.Append(contact->name + ", ");
			}
			*/
		}
		/*
		if (usersOnline != "")
		{
		microsipDlg->BaloonPopup("Users online", usersOnline, NIIF_INFO);
		AfxMessageBox("!");
		}
		*/
	}
}

#ifndef _GLOBAL_NO_MESSAGING
static void on_pager(pjsua_call_id call_id, const pj_str_t *from, const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type, const pj_str_t *body)
{
	if (IsWindow(microsipDlg->m_hWnd))
	{
		CString number=PjToStr(from, TRUE);
		CString message=PjToStr(body, TRUE);
		message.Trim();
		MessagesContact* messagesContact = microsipDlg->messagesDlg->AddTab(number);
		microsipDlg->messagesDlg->AddMessage(messagesContact, message, MSIP_MESSAGE_TYPE_REMOTE);
	}
}

static void on_pager_status(pjsua_call_id call_id, const pj_str_t *to, const pj_str_t *body, void *user_data, pjsip_status_code status, const pj_str_t *reason)
{
	if (status != 200)
	{
		if (IsWindow(microsipDlg->m_hWnd))
		{
			CString number=PjToStr(to, TRUE);
			CString message=PjToStr(reason);
			message.Trim();
			MessagesContact* messagesContact = microsipDlg->messagesDlg->AddTab(number);
			microsipDlg->messagesDlg->AddMessage(messagesContact, message);
		}
	}
}
#endif

static void on_call_transfer_status(pjsua_call_id call_id,
									int status_code,
									const pj_str_t *status_text,
									pj_bool_t final,
									pj_bool_t *p_cont)
{
	pjsua_call_info call_info;
	pjsua_call_get_info(call_id, &call_info);
	CString number = PjToStr(&call_info.remote_info, TRUE);
	MessagesContact* messagesContact = microsipDlg->messagesDlg->AddTab(number, "", FALSE, &call_info, TRUE, TRUE);

	CString str;
	str.Format("%s: %.*s",
		Translate("Transfer status"),
		(int)status_text->slen, status_text->ptr);
	if (final) {
		str.AppendFormat(" [%s]", Translate("final"));
	}
	microsipDlg->messagesDlg->AddMessage(messagesContact, str);

	if (status_code/100 == 2) {
		str = Translate("Call transfered successfully, disconnecting call");
		microsipDlg->messagesDlg->AddMessage(messagesContact, str);
		pjsua_call_hangup(call_id, 0, NULL, NULL);
		*p_cont = PJ_FALSE;
	}
}

CmicrosipDlg::~CmicrosipDlg(void)
{
	KillTimer(IDT_TIMER_0);
}

void CmicrosipDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	UnloadLangPackModule();

	PJDestroy();
	if (tnd.hWnd) {
		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
	delete this;
}

void CmicrosipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CmicrosipDlg, CDialog)
	ON_MESSAGE(MYWM_NOTIFYICON,onTrayNotify)
	ON_MESSAGE(MYWM_CREATE_RINGING,createRingingDlg)
	ON_MESSAGE(MYWM_DESTROY_RINGING,destroyRingingDlg)
	ON_MESSAGE(MYWM_UPDATE_WINDOW_TEXT,updateWindowText)
	ON_MESSAGE(MYWM_ON_CALL_STATE,onCallState)
	ON_MESSAGE(WM_POWERBROADCAST,onPowerBroadcast)
	ON_MESSAGE(WM_COPYDATA,onDialNumber)
	ON_MESSAGE(MYWM_CALL_ANSWER,callAnswer)
	ON_COMMAND(ID_ACCOUNT,OnMenuAccount)
	ON_COMMAND(ID_SETTINGS,OnMenuSettings)
#ifndef _GLOBAL_NO_LOG
	ON_COMMAND(ID_LOG,OnMenuLog)
#endif
	ON_COMMAND(ID_EXIT,OnMenuExit)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, ID_TAB, &CmicrosipDlg::OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, ID_TAB, &CmicrosipDlg::OnTcnSelchangingTab)
#ifndef _GLOBAL_CUSTOM
	ON_COMMAND(ID_WEBSITE,OnMenuWebsite)
#endif
#ifdef _GLOBAL_VIDEO
	ON_MESSAGE(MYWM_CREATE_PREVIEW,createPreviewWin)
#endif
END_MESSAGE_MAP()


// CmicrosipDlg message handlers

CmicrosipDlg::CmicrosipDlg(CWnd* pParent /*=NULL*/)
: CDialog(CmicrosipDlg::IDD, pParent)
{
	microsipDlg = this;	
	Create (IDD, pParent);
}

BOOL CmicrosipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( strcmp(theApp.m_lpCmdLine,"/hidden") == 0) {
		accountSettings.hidden = TRUE;
		theApp.m_lpCmdLine = "";
	}

	LoadLangPackModule();

	accountDlg = NULL;
	settingsDlg = NULL;
	messagesDlg = new MessagesDlg(this);
	transferDlg = NULL;

	isSubscribed = FALSE;

	m_idleCounter = 0;
	m_isAway = FALSE;

	player = -1;

#ifdef _GLOBAL_VIDEO
	previewWin = NULL;
#endif

	SetTimer(IDT_TIMER_0,5000,NULL);

	if (!accountSettings.hidden) {

		m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
		HICON iconSmall = (HICON)LoadImage(
			AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDR_MAINFRAME),
			IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED );
		PostMessage(WM_SETICON,ICON_SMALL,(LPARAM)iconSmall);

		TranslateDialog(this->m_hWnd);

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

		// TODO: Add extra initialization here

		// add tray icon
		CString str;
		str.Format("%s %s", _GLOBAL_NAME_NICE, _GLOBAL_VERSION);
		tnd.cbSize = NOTIFYICONDATA_V3_SIZE;
		tnd.hWnd = this->GetSafeHwnd();
		tnd.uID = IDR_MAINFRAME;
		tnd.uCallbackMessage = MYWM_NOTIFYICON;
		tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP; 
		tnd.hIcon = iconSmall;
		lstrcpyn(tnd.szTip, (LPCTSTR)str, sizeof(tnd.szTip));
		DWORD dwMessage = NIM_ADD;
		Shell_NotifyIcon(dwMessage, &tnd);
	} else {
		tnd.hWnd = NULL;
	}

	// set window pos
	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&screenRect,0);
	CRect rect;
	this->GetWindowRect(&rect);
	int mx = screenRect.Width()-rect.Width();
	int my = screenRect.Height()-rect.Height();

	CRect cRect;
	messagesDlg->GetWindowRect(&cRect);
	messagesDlg->SetWindowPos(NULL, mx - cRect.Width(), my, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

	this->SetWindowPos(NULL, mx, my, 0, 0, SWP_NOSIZE|SWP_NOZORDER);


	m_bar.Create(this);
	m_bar.SetIndicators(indicators,1);
	this->GetClientRect(&rect);
	m_bar.SetPaneInfo(0,ID_STATUSBAR, SBPS_NORMAL, rect.Width());
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_STATUSBAR);

	m_bar.SetPaneText( 0, "");

	CTabCtrl* tab = (CTabCtrl*) GetDlgItem(ID_TAB);
	TC_ITEM tabItem;
	tabItem.mask = TCIF_TEXT | TCIF_PARAM;

	pageDialer = new Dialer(tab);
	tabItem.pszText = Translate("Dialpad");
	tabItem.lParam = (LPARAM)pageDialer;
	tab->InsertItem( 0, &tabItem );
	pageDialer->SetWindowPos(NULL, 0, 27, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pageContacts = new Contacts(tab);
	tabItem.pszText = Translate("Contacts");
	tabItem.lParam = (LPARAM)pageContacts;
	tab->InsertItem( 1, &tabItem );
	pageContacts->SetWindowPos(NULL, 0, 27, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	switch (accountSettings.activeTab)
	{
	case 1:
		pageContacts->ShowWindow( SW_SHOW );
		tab->SetCurSel(1);
		break;
	default:
		pageDialer->ShowWindow( SW_SHOW );
		tab->SetCurSel(0);
	}

#ifdef _GLOBAL_ACCOUNT_MINI
	if (accountSettings.username=="")
	{
		onTrayNotify(NULL,WM_LBUTTONUP);
		OnMenuAccount();
	}
#else 
	if (firstRun)
	{
		onTrayNotify(NULL,WM_LBUTTONUP);
	}
#endif

	onPowerBroadcast(PBT_APMRESUMESUSPEND, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CmicrosipDlg::BaloonPopup(CString title, CString message, DWORD flags)
{
	if (tnd.hWnd) {
		lstrcpyn(tnd.szInfo, message, sizeof(tnd.szInfo));
		lstrcpyn(tnd.szInfoTitle, title, sizeof(tnd.szInfoTitle));
		tnd.uFlags = tnd.uFlags | NIF_INFO; 
		tnd.dwInfoFlags = flags;
		DWORD dwMessage = NIM_MODIFY;
		Shell_NotifyIcon(dwMessage, &tnd);
	}
}

void CmicrosipDlg::OnMenuAccount()
{
	if (!accountSettings.hidden) {
		if (!accountDlg)
		{
			accountDlg = new AccountDlg(this);
		}
		::SetForegroundWindow(accountDlg->m_hWnd);
	}
}

void CmicrosipDlg::OnMenuSettings()
{
	if (!accountSettings.hidden) {
		if (!settingsDlg)
		{
			settingsDlg = new SettingsDlg(this);
		}
		::SetForegroundWindow(settingsDlg->m_hWnd);
	}
}

#ifndef _GLOBAL_NO_LOG
void CmicrosipDlg::OnMenuLog()
{
	ShellExecute(NULL, NULL, accountSettings.logFile, NULL, NULL, SW_SHOWNORMAL);
}
#endif

void CmicrosipDlg::OnMenuExit()
{
	DestroyWindow ();
}


LRESULT CmicrosipDlg::onTrayNotify(WPARAM wParam,LPARAM lParam)
{
	UINT uMsg = (UINT) lParam; 
	switch (uMsg ) 
	{ 
	case WM_LBUTTONUP:
		if (this->IsWindowVisible())
		{
			if (wParam) {
				this->ShowWindow(SW_HIDE);
			}
		} else
		{
			if (!accountSettings.hidden) {
				this->ShowWindow(SW_SHOW);
				SetForegroundWindow();
				CTabCtrl* tab = (CTabCtrl*) GetDlgItem(ID_TAB);
				int nTab = tab->GetCurSel();
				TC_ITEM tci;
				tci.mask = TCIF_PARAM;
				tab->GetItem(nTab, &tci);
				CWnd* pWnd = (CWnd *)tci.lParam;
				pWnd->SetFocus();
			}
		}
		break;
	case WM_RBUTTONUP:
		CPoint point;    
		GetCursorPos(&point);
		CMenu menu;
		menu.LoadMenu(IDR_MENU_TRAY);
		CMenu* tracker = menu.GetSubMenu(0);
		TranslateMenu(tracker->m_hMenu);
		if (!accountSettings.enableLog) {
			tracker->EnableMenuItem(ID_LOG, MF_DISABLED | MF_GRAYED);
		}
		SetForegroundWindow();
		tracker->TrackPopupMenu( 0, point.x, point.y, this );
		PostMessage(WM_NULL, 0, 0);
		break;
	} 
	return TRUE;
}

LRESULT CmicrosipDlg::createRingingDlg(WPARAM wParam,LPARAM lParam)
{
	pjsua_call_info* call_info = (pjsua_call_info*) wParam;

	RinginDlg* ringinDlg = new RinginDlg(this);

	if (call_info->rem_vid_cnt) {
		((CButton*)ringinDlg->GetDlgItem(IDC_VIDEO))->EnableWindow(TRUE);
		ringinDlg->GotoDlgCtrl(ringinDlg->GetDlgItem(IDC_VIDEO));
	} else {
		ringinDlg->GotoDlgCtrl(ringinDlg->GetDlgItem(IDOK));
	}

	if (ringinDlgs.GetCount())
	{
		CRect rect;
		ringinDlgs.GetAt(ringinDlgs.GetCount()-1)->GetWindowRect(&rect);
		ringinDlg->SetWindowPos( NULL, rect.left-22, rect.top+22, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	ringinDlg->call_id = call_info->id;
	SIPURI sipuri;
	CStringW rab;
	CString str;

	str = PjToStr(&call_info->remote_info, TRUE);
	ParseSIPURI(str, &sipuri);
	CString name = pageContacts->GetNameByNumber(sipuri.user + "@" + sipuri.domain);
	ringinDlg->GetDlgItem(IDC_CALLER_NAME)->SetWindowText(name.GetLength()?name:(sipuri.name.GetLength()?sipuri.name:sipuri.user));

#ifndef _GLOBAL_NO_RINGIN_DETAILS
	ringinDlg->GetDlgItem(IDC_CALLER_ADDR)->SetWindowText(str);
	str.Format("%s: %s", Translate("Local"), PjToStr(&call_info->local_info, TRUE));
	ringinDlg->GetDlgItem(IDC_CALLED)->SetWindowText(str);
#endif

	ringinDlg->SetForegroundWindow();
	ringinDlgs.Add(ringinDlg);

	return TRUE;
}

LRESULT CmicrosipDlg::destroyRingingDlg(WPARAM wParam,LPARAM lParam)
{
	RinginDlg* ringinDlg = (RinginDlg *) wParam;
	ringinDlg->DestroyWindow();
	return TRUE;
}

LRESULT CmicrosipDlg::onCallState(WPARAM wParam,LPARAM lParam) {

	pjsua_call_info *call_info = (pjsua_call_info *) wParam;

	BOOL notShowWindow = FALSE;

	CString str;
	CString adder;
	switch (call_info->state)
	{
	case PJSIP_INV_STATE_CALLING:
		str = Translate("Calling");
		break;
	case PJSIP_INV_STATE_INCOMING:
		notShowWindow = TRUE;
		str = Translate( "Incoming call" );
		break;
	case PJSIP_INV_STATE_EARLY:
		notShowWindow = TRUE;
		str = Translate( PjToStr(&call_info->last_status_text).GetBuffer() );
		break;
	case PJSIP_INV_STATE_CONNECTING:
		str = Translate("Connecting");
		break;
	case PJSIP_INV_STATE_CONFIRMED:
		str = Translate("Connected");
		for (unsigned i=0;i<call_info->media_cnt;i++)
		{
			if (call_info->media[i].type == PJMEDIA_TYPE_AUDIO || call_info->media[i].type == PJMEDIA_TYPE_VIDEO)
			{
				pjsua_stream_info psi;
				if (pjsua_call_get_stream_info(call_info->id, call_info->media[i].index, &psi) == PJ_SUCCESS)
				{
					if (call_info->media[i].type == PJMEDIA_TYPE_AUDIO)
					{
						adder.AppendFormat("%s@%dkHz %dkbit/s, ",PjToStr (&psi.info.aud.fmt.encoding_name),psi.info.aud.fmt.clock_rate/1000, psi.info.aud.param->info.avg_bps/1000);
					} else 
					{
						adder.AppendFormat("%s %dkbit/s, ",
							PjToStr (&psi.info.vid.codec_info.encoding_name),
							psi.info.vid.codec_param->enc_fmt.det.vid.max_bps/1000
							);
					}
				}
			}
		}
		if (adder.GetLength()) {
			str.AppendFormat(" (%s)", adder.Left(adder.GetLength()-2) );
		}
		break;

	case PJSIP_INV_STATE_DISCONNECTED:
		call_deinit_tonegen(call_info->id);
		notShowWindow = TRUE;
		pageDialer->SetNumber("");
		if (call_info->last_status == 200)
		{
			str = Translate("Call ended");
		} else {
			str = PjToStr(&call_info->last_status_text).GetBuffer();
			if (str == "Decline")
			{
				str = "Declined";
			}
			str=Translate(str.GetBuffer());
			if (accountSettings.singleMode && (call_info->last_status != 603 || call_info->role == PJSIP_ROLE_UAC))
			{
				microsipDlg->BaloonPopup(str, str, NIIF_INFO);
			}
		}
		break;
	}

	if (call_info->role==PJSIP_ROLE_UAC)
	{
		if (call_info->last_status == 180)
		{
			microsipDlg->OnTimer(IDT_TIMER_1);
			microsipDlg->SetTimer(IDT_TIMER_1,4500,NULL);
		} else
		{
			if (microsipDlg->KillTimer(IDT_TIMER_1))
			{
				PlaySound(NULL, 0, 0);
			}
		}
	}

	CString number = PjToStr(&call_info->remote_info, TRUE);
	MessagesContact* messagesContact = microsipDlg->messagesDlg->AddTab(number, "", FALSE, call_info, accountSettings.singleMode || notShowWindow, call_info->state == PJSIP_INV_STATE_DISCONNECTED);
	if (messagesContact && str!="")
	{
		microsipDlg->messagesDlg->AddMessage(messagesContact, str);
	}
	if (call_info->state == PJSIP_INV_STATE_DISCONNECTED)
	{
		microsipDlg->messagesDlg->OnEndCall(call_info);
#ifdef _GLOBAL_VIDEO
		if (microsipDlg->previewWin && pjsua_call_get_count()<2)
		{
			microsipDlg->previewWin->PostMessage(WM_CLOSE, NULL, NULL);
		}
#endif
	}

	if (accountSettings.singleMode)
	{
		if (call_info->state == PJSIP_INV_STATE_DISCONNECTED)
		{
			microsipDlg->pageDialer->UpdateCallButton(FALSE, pjsua_call_get_count()-1);
		} else {
			CTabCtrl* tab = (CTabCtrl*) microsipDlg->GetDlgItem(ID_TAB);
			if (tab->GetCurSel())
			{
				LRESULT pResult;
				microsipDlg->OnTcnSelchangingTab(NULL, &pResult);
				tab->SetCurSel(0);
				microsipDlg->OnTcnSelchangeTab(NULL, &pResult);
			}
			microsipDlg->pageDialer->SetNumber(messagesContact->number);
			microsipDlg->pageDialer->GotoDlgCtrl(microsipDlg->pageDialer->GetDlgItem(IDC_CALL));
		}
	}

	if (call_info->role==PJSIP_ROLE_UAS)
	{ 
		if (call_info->state == PJSIP_INV_STATE_DISCONNECTED || call_info->state == PJSIP_INV_STATE_CONFIRMED)
		{
			int count = microsipDlg->ringinDlgs.GetCount();
			for (int i = 0; i < count; i++ )
			{
				if ( call_info->id == microsipDlg->ringinDlgs.GetAt(i)->call_id)
				{
					microsipDlg->PostMessage(MYWM_DESTROY_RINGING, (WPARAM)microsipDlg->ringinDlgs.GetAt(i), 0);
					microsipDlg->ringinDlgs.RemoveAt(i);
					if (count==1)
					{
						PlaySound(NULL, 0, 0);
					}
					break;
				}
			}
		}
	}
#ifdef _GLOBAL_BALANCE
	if (call_info->state == PJSIP_INV_STATE_DISCONNECTED && pjsua_call_get_count()<2)
	{
		SetTimer(IDT_TIMER_BALANCE,2000,NULL);
	}
#endif
	return TRUE;
}

LRESULT CmicrosipDlg::onPowerBroadcast(WPARAM wParam,LPARAM lParam)
{
	if (wParam == PBT_APMRESUMESUSPEND)
	{
		BOOL err = TRUE; 	
		PHOSTENT hostinfo;
		char name[255];
		char *ip;
		int nCount = 0;
		WSAData ws;
		int wsaError = -1;
		while (err && nCount < 3)
		{
			err = FALSE;
			if (wsaError) {
				wsaError = WSAStartup(MAKEWORD(2,2), &ws);
				if (wsaError==WSASYSNOTREADY) {
					err = TRUE;
				} else if (wsaError) {
					break;
				}
			}
			if (!err) {
				if( (hostinfo = gethostbyname(NULL)) != NULL )
				{
					if (hostinfo->h_addr_list[0] && !hostinfo->h_addr_list[1])
					{
						ip = inet_ntoa(*(
						struct in_addr *)hostinfo->h_addr_list[0]);
						if ( !strcmp(ip, "127.0.0.1") )
						{
							err = TRUE;
#ifdef _GLOBAL_LOCAL_SIP_SERVER
						} else {
							accountSettings.server = ip;
							accountSettings.domain = ip;
#endif
						}
					}
				}
			}
			if (err) {
				Sleep(3000);
				nCount++;
			}
		}
		PJCreate();
		PJAccountAdd();
	} else if (wParam == PBT_APMSUSPEND)
	{
		PJDestroy();
	}
	return TRUE;
}

LRESULT CmicrosipDlg::updateWindowText(WPARAM wParam, LPARAM lParam)
{
	UpdateWindowText();
	return TRUE;
}

void CmicrosipDlg::OnTimer (UINT TimerVal)
{
#ifdef _GLOBAL_BALANCE
	if (TimerVal == IDT_TIMER_BALANCE)
	{
		KillTimer(IDT_TIMER_BALANCE);
		pjsua_acc_set_registration(account, TRUE);
	} else
#endif		
		if (TimerVal == IDT_TIMER_0)
		{
			POINT coord;
			GetCursorPos(&coord);
			if (m_mousePos.x != coord.x || m_mousePos.y != coord.y)
			{
				m_idleCounter = 0;
				m_mousePos = coord;
				if (m_isAway)
				{
					PublishStatus();
				}
			} else {
				m_idleCounter++;
				if (m_idleCounter == 120)
				{
					PublishStatus(FALSE);
				}
			}
		} else 
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_RINGOUT),  GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_NOSTOP);
		}
}

void CmicrosipDlg::PJCreate()
{
	// check updates
#ifndef _GLOBAL_CUSTOM
	if (accountSettings.updatesInterval != "never") 
	{
		CTime t = CTime::GetCurrentTime();
		time_t time = t.GetTime();
		int days;
		if (accountSettings.updatesInterval =="daily")
		{
			days = 1;
		} else if (accountSettings.updatesInterval =="monthly")
		{
			days = 30;
		} else if (accountSettings.updatesInterval =="quarterly")
		{
			days = 90;
		} else
		{
			days = 7;
		}
		if (accountSettings.checkUpdatesTime + days * 86400 < time)
		{
			CheckUpdates();
			accountSettings.checkUpdatesTime = time;
			accountSettings.Save();
		}
	}
#endif

	// pj create
	pj_status_t status;
	pjsua_config         ua_cfg;
	pjsua_media_config   media_cfg;
	pjsua_transport_config cfg;   

	// Must create pjsua before anything else!
	status = pjsua_create();
	if (status != PJ_SUCCESS)
	{
		return;
	}

	// Initialize configs with default settings.
	pjsua_config_default(&ua_cfg);
	pjsua_media_config_default(&media_cfg);

	CString userAgent;
	userAgent.Format("%s/%s", _GLOBAL_NAME_NICE, _GLOBAL_VERSION);
	ua_cfg.user_agent = pj_str( userAgent.GetBuffer() );

	ua_cfg.cb.on_reg_state2=&on_reg_state2;
	ua_cfg.cb.on_call_state=&on_call_state;
	ua_cfg.cb.on_call_media_state = &on_call_media_state;
	ua_cfg.cb.on_incoming_call = &on_incoming_call;
	ua_cfg.cb.on_nat_detect= &on_nat_detect;
	ua_cfg.cb.on_buddy_state = &on_buddy_state;
#ifndef _GLOBAL_NO_MESSAGING
	ua_cfg.cb.on_pager = &on_pager;
	ua_cfg.cb.on_pager_status = &on_pager_status;
#endif
	ua_cfg.cb.on_call_transfer_status = &on_call_transfer_status;

	ua_cfg.srtp_secure_signaling=0;

	if (accountSettings.srtp =="optional")
	{
		ua_cfg.use_srtp = PJMEDIA_SRTP_OPTIONAL;
	} else if (accountSettings.srtp =="mandatory")
	{
		ua_cfg.use_srtp = PJMEDIA_SRTP_MANDATORY;
	} else
	{
		ua_cfg.use_srtp = PJMEDIA_SRTP_DISABLED;
	}

	if (accountSettings.stun.GetLength()) 
	{
		ua_cfg.stun_srv_cnt=1;
		ua_cfg.stun_srv[0] = pj_str( accountSettings.stun.GetBuffer() );
	}

	media_cfg.enable_ice = accountSettings.ice ? PJ_TRUE : PJ_FALSE;
	media_cfg.no_vad = accountSettings.disableVAD ? PJ_TRUE : PJ_FALSE;

#ifdef _GLOBAL_PTIME
	media_cfg.ptime = _GLOBAL_PTIME;
#endif
#ifdef _GLOBAL_JB_MAX
	media_cfg.jb_max=_GLOBAL_JB_MAX;
#endif
#ifdef _GLOBAL_JB_MAX_PRE
	media_cfg.jb_max_pre=_GLOBAL_JB_MAX_PRE;
#endif
#ifdef _GLOBAL_JB_INIT
	media_cfg.jb_init=_GLOBAL_JB_INIT;
#endif


	// Initialize pjsua
#ifndef _GLOBAL_NO_LOG
	if (accountSettings.enableLog) {
		pjsua_logging_config log_cfg;
		pjsua_logging_config_default(&log_cfg);
		log_cfg.log_filename = pj_str(accountSettings.logFile.GetBuffer());
		status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
	} else {
		status = pjsua_init(&ua_cfg, NULL, &media_cfg);
	}
#else
	status = pjsua_init(&ua_cfg, NULL, &media_cfg);
#endif

	if (status != PJ_SUCCESS)
	{
		pjsua_destroy();
		return;
	}

	// Start pjsua
	status = pjsua_start();
	if (status != PJ_SUCCESS)
	{
		pjsua_destroy();
		return;
	}

	// Set snd devices
	int audio_input=-1;
	int audio_output=-1;
	unsigned count = 64;
	pjmedia_aud_dev_info aud_dev_info[64];
	pjsua_enum_aud_devs(aud_dev_info, &count);
	for (unsigned i=0;i<count;i++)
	{
		if (aud_dev_info[i].input_count && !accountSettings.audioInputDevice.Compare(aud_dev_info[i].name)) {
			audio_input = i;
		}
		if (aud_dev_info[i].output_count && !accountSettings.audioOutputDevice.Compare(aud_dev_info[i].name)) {
			audio_output = i;
		}
	}
	if (audio_input != -1 || audio_output != -1)
	{
		ShowErrorMessage(pjsua_set_snd_dev(audio_input, audio_output));
	}

	//Set aud codecs prio
#ifdef _GLOBAL_CODECS_ENABLED
	if (accountSettings.audioCodecs=="")
	{
		accountSettings.audioCodecs = _GLOBAL_CODECS_ENABLED;
	}
#endif
	if (accountSettings.audioCodecs.GetLength())
	{
		pjsua_codec_info codec_info[64];
		unsigned count = 64;
		pjsua_enum_codecs(codec_info, &count);
		for (unsigned i=0;i<count;i++)
		{
			pjsua_codec_set_priority(&codec_info[i].codec_id,0);
		}
		CString resToken;
		int curPos = 0;
		int i = 255;
		resToken= accountSettings.audioCodecs.Tokenize(" ",curPos);
		while (resToken.GetLength())
		{
			pj_str_t codec_id = pj_str(StrToPj(resToken));
			pjsua_codec_set_priority(&codec_id, i);
			resToken= accountSettings.audioCodecs.Tokenize(" ",curPos);
			i--;
		}
	}
#ifdef _GLOBAL_VIDEO
	//Set vid codecs prio
	if (accountSettings.videoCodec.GetLength())
	{
		pj_str_t codec_id = pj_str(StrToPj(accountSettings.videoCodec));
		pjsua_vid_codec_set_priority(&codec_id,255);
	}
	int bitrate;
	if (accountSettings.disableH264) {
		pjsua_vid_codec_set_priority(&pj_str("H264/97"),0);
	} else
	{
		const pj_str_t codec_id = {"H264", 4};
		pjmedia_vid_codec_param param;
		pjsua_vid_codec_get_param(&codec_id, &param);
		param.enc_fmt.det.vid.size.w = 640;
		param.enc_fmt.det.vid.size.h = 480;
		param.enc_fmt.det.vid.fps.num = 30;
		param.enc_fmt.det.vid.fps.denum = 1;
		if (atoi(accountSettings.bitrateH264)) {
			bitrate = 1000 * atoi(accountSettings.bitrateH264);
			param.enc_fmt.det.vid.avg_bps = bitrate;
			param.enc_fmt.det.vid.max_bps = bitrate;
		}
		param.dec_fmt.det.vid.size.w = 640;
		param.dec_fmt.det.vid.size.h = 480;
		param.dec_fmt.det.vid.fps.num = 20;
		param.dec_fmt.det.vid.fps.denum = 1;

		param.dec_fmtp.cnt = 2;
		param.dec_fmtp.param[0].name = pj_str("profile-level-id");
		param.dec_fmtp.param[0].val = pj_str("42e01e");
		param.dec_fmtp.param[1].name = pj_str("packetization-mode");
		param.dec_fmtp.param[1].val = pj_str("1");
		pjsua_vid_codec_set_param(&codec_id, &param);
	}
	if (accountSettings.disableH263) {
		pjsua_vid_codec_set_priority(&pj_str("H263-1998/96"),0);
	} else {
		if (atoi(accountSettings.bitrateH263)) {
			const pj_str_t codec_id = {"H263", 4};
			pjmedia_vid_codec_param param;
			pjsua_vid_codec_get_param(&codec_id, &param);
			bitrate = 1000 * atoi(accountSettings.bitrateH263);
			param.enc_fmt.det.vid.avg_bps = bitrate;
			param.enc_fmt.det.vid.max_bps = bitrate;
			pjsua_vid_codec_set_param(&codec_id, &param);
		}
	}
#endif

	// Create transport
	transport_udp = -1;
	transport_tcp = -1;
	transport_tls = -1;

	pjsua_transport_config_default(&cfg);
	cfg.public_addr = pj_str( accountSettings.publicAddr.GetBuffer() );

#ifdef _GLOBAL_DSCP_CONTROL
	cfg.qos_params.flags = PJ_QOS_PARAM_HAS_DSCP;
	cfg.qos_params.dscp_val = _GLOBAL_DSCP_CONTROL;
#endif

	int listenPort = accountSettings.listenPort!="" ? atoi(accountSettings.listenPort) : 5060;

	cfg.port = listenPort;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &transport_udp);
	if (status != PJ_SUCCESS)
	{
		cfg.port=0;
		pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, &transport_udp);
	}
	cfg.port = listenPort;
	status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, &transport_tcp);
	if (status != PJ_SUCCESS)
	{
		cfg.port=0;
		pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, &transport_tcp);
	}
	cfg.port = listenPort + 1;
	status = pjsua_transport_create(PJSIP_TRANSPORT_TLS, &cfg, &transport_tls);
	if (status != PJ_SUCCESS)
	{
		cfg.port=0;
		pjsua_transport_create(PJSIP_TRANSPORT_TLS, &cfg, &transport_tls);
	}
}

void CmicrosipDlg::PJDestroy()
{
	PJAccountDelete();
	call_deinit_tonegen(-1);
	pjsua_destroy();
}

void CmicrosipDlg::PJAccountDelete()
{
	PresenceUnsubsribe();
	if (pjsua_acc_is_valid(account))
	{
		pjsua_acc_set_online_status(account,PJ_FALSE);
		pj_thread_sleep(1000);
		pjsua_acc_del(account);
	}
	pjsua_acc_id local_acc_id = pjsua_acc_get_default();
	if (pjsua_acc_is_valid(local_acc_id))
	{
		pjsua_acc_del(local_acc_id);
	}
}

void CmicrosipDlg::PJAccountAdd()
{
	pjsua_acc_config acc_cfg;
	pj_status_t status;

	transport = MSIP_TRANSPORT_AUTO;
	account = -1;

	BOOL isLocal = accountSettings.server=="" || accountSettings.username=="" || accountSettings.domain=="";

	CString title = _GLOBAL_NAME_NICE;
	if (accountSettings.displayName.GetLength())
	{
		title.Append( " - " + accountSettings.displayName);
	} else if (accountSettings.username.GetLength())
	{
		title.Append( " - " + accountSettings.username);
	}
	SetWindowText(title);

	pjsua_acc_config_default(&acc_cfg);
	acc_cfg.allow_contact_rewrite=PJ_FALSE;
	acc_cfg.publish_enabled = accountSettings.publish ? PJ_TRUE : PJ_FALSE;
#ifdef _GLOBAL_KEEPALIVE_UDP
	acc_cfg.ka_interval = _GLOBAL_KEEPALIVE_UDP;
#endif

#ifdef _GLOBAL_VIDEO
	acc_cfg.vid_in_auto_show = PJ_TRUE;
	acc_cfg.vid_out_auto_transmit = PJ_TRUE;
	acc_cfg.vid_cap_dev = VideoCaptureDeviceId();
	acc_cfg.vid_wnd_flags = PJMEDIA_VID_DEV_WND_BORDER | PJMEDIA_VID_DEV_WND_RESIZABLE;
#endif

#ifdef _GLOBAL_DSCP_MEDIA
	acc_cfg.rtp_cfg.qos_params.flags = PJ_QOS_PARAM_HAS_DSCP;
	acc_cfg.rtp_cfg.qos_params.dscp_val = _GLOBAL_DSCP_MEDIA;
#endif

	pjsua_transport_info info;
	if (accountSettings.transport=="udp" && transport_udp!=-1)
	{
		acc_cfg.transport_id = transport_udp;
	} else if (accountSettings.transport=="tcp" && transport_tcp!=-1)
	{
		transport = MSIP_TRANSPORT_TCP;
	} else if (accountSettings.transport=="tls" && transport_tls!=-1)
	{
		transport = MSIP_TRANSPORT_TLS;
	}

	if (isLocal || !accountSettings.disableLocalAccount)
	{
		pjsua_acc_id acc_id;
		//pjsua_acc_add_local(NULL, PJ_TRUE, &acc_id);
		acc_cfg.priority--;
		pjsua_transport_data *t = &pjsua_var.tpdata[0];

		CString localURI;
		if (accountSettings.displayName.GetLength())
		{
			localURI = "\"" + accountSettings.displayName + "\" ";
		}
		localURI.AppendFormat("<sip:%s>", PjToStr(&t->local_name.host));

		acc_cfg.id = pj_str(StrToPj(localURI));
		pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
		acc_cfg.priority++;
	}

	if (!isLocal)
	{
		UpdateWindowText(Translate("Connecting..."));

		CString localURI;
		if (accountSettings.displayName.GetLength())
		{
			localURI = "\"" + accountSettings.displayName + "\" ";
		}
		localURI += GetSIPURI(accountSettings.username);

		acc_cfg.id = pj_str(StrToPj(localURI));
		acc_cfg.cred_count = 1;
		acc_cfg.cred_info[0].username = pj_str( accountSettings.authID!=""? accountSettings.authID.GetBuffer() : accountSettings.username.GetBuffer() );
		acc_cfg.cred_info[0].realm = pj_str("*");
		acc_cfg.cred_info[0].scheme = pj_str("Digest");
		acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
		acc_cfg.cred_info[0].data = pj_str( accountSettings.password.GetBuffer() );

		CString regURI;
		regURI.Format("sip:%s",accountSettings.server);
		AddTransportSuffix(regURI);
		acc_cfg.reg_uri = pj_str( regURI.GetBuffer() );

		CString proxy;		
		if (accountSettings.proxy.GetLength())
		{
			acc_cfg.proxy_cnt = 1;
			proxy.Format("sip:%s",accountSettings.proxy);
			AddTransportSuffix(proxy);
			acc_cfg.proxy[0] = pj_str( proxy.GetBuffer() );
		}

		status = pjsua_acc_add(&acc_cfg, PJ_FALSE, &account);

		if (status == PJ_SUCCESS)
		{
			PublishStatus();
		} else
		{
			ShowErrorMessage(status);
			UpdateWindowText();
		}
	} else
	{
		UpdateWindowText();
	}

	if (strlen(theApp.m_lpCmdLine))
	{
		if (!isLocal)
		{
			dialNumberDelayed = theApp.m_lpCmdLine;
		} else 
		{
			DialNumber(theApp.m_lpCmdLine);
		}
		theApp.m_lpCmdLine = "";
	}
}

void CmicrosipDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl* tab = (CTabCtrl*) GetDlgItem(ID_TAB);
	int nTab = tab->GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	tab->GetItem(nTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_SHOW);
	pWnd->SetFocus();
	accountSettings.activeTab=nTab;
	accountSettings.Save();
	*pResult = 0;
}

void CmicrosipDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl* tab = (CTabCtrl*) GetDlgItem(ID_TAB);
	int nTab = tab->GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	tab->GetItem(nTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_HIDE);
	*pResult = 0;
}

void CmicrosipDlg::UpdateWindowText(CString text)
{
	CString str;
	int icon = IDI_OFFLINE;
	if (text=="") {
		if (pjsua_acc_is_valid(account))
		{
			pjsua_acc_info info;
			pjsua_acc_get_info(account,&info);
			str = PjToStr(&info.status_text);
			if ( str != "Default status message" ) {
				if (str=="OK")
				{
					str = m_isAway ? "Away" : "Online";
					if (balance!="") {
						str.AppendFormat(" - your balance %s", balance);
					}
					icon = m_isAway ? IDI_AWAY : IDI_ONLINE;
					PresenceSubsribe();
					if (dialNumberDelayed!="")
					{
						DialNumber(dialNumberDelayed);
						dialNumberDelayed="";
					}
				} else if (str == "In Progress")
				{
					str = "Connecting...";
				} else if (info.status == 401 || info.status == 403)
				{
					onTrayNotify(NULL,WM_LBUTTONUP);
					OnMenuAccount();
					str = "Incorrect password";
				}
				str = Translate(str.GetBuffer());
			} else {
				str.Format( "%s: %d", Translate("Response code"),info.status );
			}
		} else {
			str= Translate("Local");
			icon = IDI_UNKNOWN;
		}
	} else
	{
		str = text;
	}
	m_bar.SetPaneText( 0, str);
	HICON hIcon = (HICON)LoadImage(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(icon),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED );
	m_bar.GetStatusBarCtrl().SetIcon(0, hIcon);
}

CString CmicrosipDlg::GetErrorMessage(pj_status_t status)
{
	CString str;
	char *buf = str.GetBuffer(PJ_ERR_MSG_SIZE-1);
	pj_strerror(status, buf, PJ_ERR_MSG_SIZE);
	str.ReleaseBuffer();
	int i = str.Find( '(' );
	if (i!=-1)
	{
		str = str.Left(i-1);
	}
	if (str == "Invalid Request URI") {
		str = "Invalid URI";
	}
	return Translate(str.GetBuffer());
}


BOOL CmicrosipDlg::ShowErrorMessage(pj_status_t status)
{
	if (status!=PJ_SUCCESS)
	{
		AfxMessageBox(GetErrorMessage(status));
		return TRUE;
	} else {
		return FALSE;
	}
}

CString CmicrosipDlg::GetSIPURI(CString str, BOOL isSimple)
{
	CString rab = str;
	rab.MakeLower();
	int pos = rab.Find("sip:");
	if (pos==-1)
	{
		str="sip:"+str;
	}
	pos = str.Find("@");
	if (accountSettings.domain!="" && pos==-1)
	{
		str.Append("@"+accountSettings.domain);
	}
	if (str.GetAt(str.GetLength()-1)=='>')
	{
		str = str.Left(str.GetLength()-1);
		if (!isSimple) {
			AddTransportSuffix(str);
		}
		str += ">";
	} else {
		if (!isSimple) {
			AddTransportSuffix(str);
		}
		str = "<" + str + ">";
	}
	return str;
}


void CmicrosipDlg::AddTransportSuffix(CString &str)
{
	switch (transport)
	{ 
	case MSIP_TRANSPORT_TCP:
		str.Append(";transport=tcp");
		break;
	case MSIP_TRANSPORT_TLS:
		str.Append(";transport=tls");
		break;
	}
}

void CmicrosipDlg::PresenceSubsribe()
{
	if (!isSubscribed && pageContacts)
	{
		CListCtrl *list= (CListCtrl*)pageContacts->GetDlgItem(IDC_CONTACTS);
		int n = list->GetItemCount();
		for (int i=0; i<n; i++) {
			Contact *pContact = (Contact *) list->GetItemData(i);
			if (pContact->presence)
			{
				pjsua_buddy_id p_buddy_id;
				pjsua_buddy_config buddy_cfg;
				pjsua_buddy_config_default(&buddy_cfg);
				buddy_cfg.subscribe=PJ_TRUE;
				CString uri = GetSIPURI(pContact->number);
				buddy_cfg.uri = pj_str( uri.GetBuffer() );
				buddy_cfg.user_data = (void *)pContact;
				pjsua_buddy_add(&buddy_cfg, &p_buddy_id);
			}
		}
		isSubscribed=TRUE;
	}
}

void CmicrosipDlg::PresenceSubsribeOne(Contact *pContact)
{
	if (isSubscribed)
	{
		pjsua_buddy_id p_buddy_id;
		pjsua_buddy_config buddy_cfg;
		pjsua_buddy_config_default(&buddy_cfg);
		buddy_cfg.subscribe=PJ_TRUE;
		CString uri = GetSIPURI(pContact->number);
		buddy_cfg.uri = pj_str( uri.GetBuffer() );
		buddy_cfg.user_data = (void *)pContact;
		pjsua_buddy_add(&buddy_cfg, &p_buddy_id);
	}
}

void CmicrosipDlg::PresenceUnsubsribe()
{
	pjsua_buddy_id ids[PJSUA_MAX_BUDDIES];
	unsigned count = PJSUA_MAX_BUDDIES;
	pjsua_enum_buddies(ids,&count);
	for (unsigned i=0;i<count;i++)
	{
		pjsua_buddy_del(ids[i]);
	}
	if (pageContacts)
	{
		CListCtrl *list= (CListCtrl *)pageContacts->GetDlgItem(IDC_CONTACTS);
		int n = list->GetItemCount();
		for (int i=0; i<n; i++)
		{
			list->SetItem(i, 0, LVIF_IMAGE, 0, 0, 0, 0, 0);
		}
	}
	isSubscribed=FALSE;
}

void CmicrosipDlg::PresenceUnsubsribeOne(Contact *pContact)
{
	pjsua_buddy_id ids[PJSUA_MAX_BUDDIES];
	unsigned count = PJSUA_MAX_BUDDIES;
	pjsua_enum_buddies(ids,&count);
	for (unsigned i=0;i<count;i++)
	{
		if ((Contact *)pjsua_buddy_get_user_data(ids[i])==pContact)
		{
			pjsua_buddy_del(ids[i]);
			break;
		}
	}
}

void CmicrosipDlg::PublishStatus(BOOL online)
{
	if (pjsua_acc_is_valid(account))
	{
		pjrpid_element pr;
		pr.type = PJRPID_ELEMENT_TYPE_PERSON;
		pr.id = pj_str(NULL);
		pr.note = pj_str(NULL);
		pr.activity = online ? PJRPID_ACTIVITY_UNKNOWN : PJRPID_ACTIVITY_AWAY;
		pjsua_acc_set_online_status2(account, PJ_TRUE, &pr);
		m_isAway = !online;
		UpdateWindowText();
	}
}

LRESULT CmicrosipDlg::onDialNumber(WPARAM wParam,LPARAM lParam)
{
	COPYDATASTRUCT *s = (COPYDATASTRUCT*)lParam;
	if (s)
	{
		BOOL isLocal = accountSettings.server=="" || accountSettings.username=="" || accountSettings.domain=="";
		if (isLocal || pjsua_acc_is_valid(account)) {
			DialNumber((LPTSTR)s->lpData);
		} else
		{
			dialNumberDelayed = (LPTSTR)s->lpData;
		}
	}
	return TRUE;
}

void CmicrosipDlg::DialNumber(CString number)
{
	if (number!="")
	{
		pageDialer->SetNumber(number);
		pageDialer->OnBnClickedCall();
	}
}

LRESULT CmicrosipDlg::callAnswer(WPARAM wParam,LPARAM lParam)
{
	pjsua_call_id call_id = wParam;
	pjsua_call_setting call_setting;
	pjsua_call_setting_default(&call_setting);
	call_setting.vid_cnt=lParam ? 1:0;
	call_setting.flag = 0;
	pjsua_call_answer2(call_id, &call_setting, 200, NULL, NULL);
	return TRUE;
}


#ifndef _GLOBAL_CUSTOM
void CmicrosipDlg::OnMenuWebsite()
{
	OpenURL("http://microsip.org.ua/");
}
void CmicrosipDlg::CheckUpdates()
{
	CInternetSession session;
	try {
		CHttpFile* pFile;
		CString url = "http://update.microsip.org.ua/?version=";
		url.Append(_GLOBAL_VERSION);
#ifndef _GLOBAL_VIDEO
		url.Append("&lite=1");
#endif
		pFile = (CHttpFile*)session.OpenURL(url);
		if (pFile)
		{
			DWORD status;
			pFile->QueryInfoStatusCode(status);
			if (status == 202)
			{
				if (::MessageBox(this->m_hWnd, Translate("Do you want to update MicroSIP?"), Translate("Update available"), MB_YESNO|MB_ICONQUESTION) == IDYES)
				{
					OpenURL("http://microsip.org.ua/downloads");
				}
			}
		}
	} catch (CInternetException *e) {}
}
#endif

#ifdef _GLOBAL_VIDEO
int CmicrosipDlg::VideoCaptureDeviceId(CString name)
{
	unsigned count = 64;
	pjmedia_vid_dev_info vid_dev_info[64];
	pjsua_vid_enum_devs(vid_dev_info, &count);
	for (unsigned i=0;i<count;i++)
	{
		if (vid_dev_info[i].fmt_cnt && (vid_dev_info[i].dir==PJMEDIA_DIR_ENCODING || vid_dev_info[i].dir==PJMEDIA_DIR_ENCODING_DECODING))
		{
			if ((name!="" && name ==vid_dev_info[i].name)
				||
				(name=="" && accountSettings.videoCaptureDevice ==vid_dev_info[i].name))
			{
				return vid_dev_info[i].id;
			}
		}
	}
	return PJMEDIA_VID_DEFAULT_CAPTURE_DEV;
}

LRESULT CmicrosipDlg::createPreviewWin(WPARAM wParam,LPARAM lParam)
{
	if (!previewWin) {
		previewWin = new Preview(this);
	}
	previewWin->Start(VideoCaptureDeviceId());
	return TRUE;
}
#endif