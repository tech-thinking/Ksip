#pragma once

#include "resource.h"

#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

CStringW UTF8ToWideChar(char* utf8);

class RinginDlg: public CDialog
{
public:
	RinginDlg(CWnd* pParent = NULL);	// standard constructor
	~RinginDlg();
	enum { IDD = IDD_RINGIN };
	pjsua_call_id call_id;
	CFont m_font;
private:
	void CallAccept(BOOL hasVideo = FALSE);
protected:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedVideo();
};
