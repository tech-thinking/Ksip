#pragma once

#include "resource.h"
#include "global.h"
#include "ClosableTabCtrl.h"

#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

class MessagesDlg :
	public CDialog
{
	CFont fontList;
	CFont fontMessage;
	PARAFORMAT2 para;
	CClosableTabCtrl tabComponent;
public:

	MessagesDlg(CWnd* pParent = NULL);	// standard constructor
	~MessagesDlg();
	enum { IDD = IDD_MESSAGES };

	MessagesContact* AddTab(CString number, CString name = "", BOOL activate = FALSE, pjsua_call_info *call_info = NULL, BOOL notShowWindow = FALSE, BOOL ifExists = FALSE);
	void OnChangeTab(pjsua_call_info *p_call_info = NULL);
	void OnEndCall(pjsua_call_info *call_info);
	void Call();
	void AddMessage(MessagesContact* messagesContact, CString message, int type = MSIP_MESSAGE_TYPE_SYSTEM);
	MessagesContact* GetMessageContact(int i = -1);

	void UpdateHoldButton(pjsua_call_info *call_info);	

	CClosableTabCtrl* tab;
private:
	void UpdateCallButton(BOOL active = FALSE, pjsua_call_info *call_info = NULL);
	void CallStart(BOOL hasVideo = FALSE);

protected:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnMsgfilterMessage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnCloseTab(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedCallEnd();
	afx_msg void OnBnClickedVideoCall();
	afx_msg void OnBnClickedHold();
	afx_msg void OnBnClickedTransfer();
	afx_msg void OnBnClickedEnd();
};