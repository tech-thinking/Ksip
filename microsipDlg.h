#pragma once

#include "const.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

#ifdef NDEBUG
#ifdef _GLOBAL_VIDEO
#pragma comment(lib, "libpjproject-i386-Win32-vc8-Release-Static-Video.lib")
#else
#pragma comment(lib, "libpjproject-i386-Win32-vc8-Release-Static-NoVideo.lib")
#endif
#else
#ifdef _GLOBAL_VIDEO
#pragma comment(lib, "libpjproject-i386-Win32-vc8-Debug-Static-Video.lib")
#else
#pragma comment(lib, "libpjproject-i386-Win32-vc8-Debug-Static-NoVideo.lib")
#endif
#endif

#include "langpack.h"

#include "RinginDlg.h"
#include "AccountDlg.h"
#include "SettingsDlg.h"
#include "MessagesDlg.h"

#include "Contacts.h"
#include "Dialer.h"
#include "Preview.h"
#include "Transfer.h"

enum EUserWndMessages
{
	MYWM_FIRST_USER_MSG = (WM_USER + 0x100 + 1),

	MYWM_NOTIFYICON,

	MYWM_CREATE_RINGING,
	MYWM_DESTROY_RINGING,
	MYWM_UPDATE_WINDOW_TEXT,
	MYWM_CREATE_PREVIEW,
	MYWM_CALL_ANSWER,
	MYWM_ON_CALL_STATE,
	MYWM_PLAY_SOUND,

	IDT_TIMER_0,
	IDT_TIMER_1,
	IDT_TIMER_BALANCE,

	UM_CLOSETAB,
	UM_DBLCLICKTAB,
	UM_QUERYTAB

};

// CmicrosipDlg dialog
class CmicrosipDlg : public CDialog
{
	// Construction
public:
	CmicrosipDlg(CWnd* pParent = NULL);	// standard constructor
	~CmicrosipDlg();

	// Dialog Data
	enum { IDD = IDD_MICROSIP_DIALOG };
	AccountDlg* accountDlg;
	SettingsDlg* settingsDlg;
	MessagesDlg* messagesDlg;
	Transfer* transferDlg;

	Dialer* pageDialer;
	Contacts* pageContacts;

	BOOL notStopRinging;
	int transport;
	BOOL isSubscribed;
	CArray <RinginDlg*> ringinDlgs;
	CString dialNumberDelayed;
	CString balance;

	pjsua_transport_id transport_udp;
	pjsua_transport_id transport_tcp;
	pjsua_transport_id transport_tls;
	pjsua_acc_id account;
	pjsua_player_id player;
	
	void PJCreate();
	void PJDestroy();
	void PJAccountAdd();
	void PJAccountDelete();
	void UpdateWindowText(CString = NULL);
	CString GetErrorMessage(pj_status_t status);
	BOOL ShowErrorMessage(pj_status_t status);
	CString GetSIPURI(CString str, BOOL isSimple = FALSE);
	void AddTransportSuffix(CString &str);
	void PresenceSubsribe();
	void PresenceSubsribeOne(Contact *pContact);
	void PresenceUnsubsribe();
	void PresenceUnsubsribeOne(Contact *pContact);
	void PublishStatus(BOOL online = TRUE);
	void BaloonPopup(CString title, CString message, DWORD flags = NIIF_WARNING);
	void DialNumber(CString number);

#ifdef _GLOBAL_VIDEO
	Preview* previewWin;
	int VideoCaptureDeviceId(CString name="");
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
protected:
	HICON m_hIcon;
	NOTIFYICONDATA tnd;
	CStatusBar m_bar;

	POINT m_mousePos;
	int m_idleCounter;
	BOOL m_isAway;

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy( );

	// Generated message map functions
	afx_msg LRESULT onTrayNotify(WPARAM, LPARAM);
	afx_msg LRESULT createRingingDlg(WPARAM, LPARAM);
	afx_msg LRESULT destroyRingingDlg(WPARAM, LPARAM);
	afx_msg LRESULT callAnswer(WPARAM,LPARAM);
	afx_msg LRESULT onPowerBroadcast(WPARAM, LPARAM);
	afx_msg LRESULT updateWindowText(WPARAM, LPARAM);
	afx_msg LRESULT onCallState(WPARAM, LPARAM);
	afx_msg LRESULT onDialNumber(WPARAM, LPARAM);
	afx_msg LRESULT CreationComplete(WPARAM, LPARAM);
#ifdef _GLOBAL_VIDEO
	afx_msg LRESULT createPreviewWin(WPARAM, LPARAM);
#endif
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuAccount();
	afx_msg void OnMenuSettings();
#ifndef _GLOBAL_NO_LOG	
	afx_msg void OnMenuLog();
#endif	
	afx_msg void OnMenuExit();
	afx_msg void OnTimer (UINT TimerVal);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
#ifndef _GLOBAL_CUSTOM
	afx_msg void OnMenuWebsite();
	afx_msg void CheckUpdates();
#endif
};