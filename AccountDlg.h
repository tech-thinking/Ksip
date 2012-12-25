#pragma once

#include "resource.h"
#include "const.h"

class AccountDlg :
	public CDialog
{
public:
	AccountDlg(CWnd* pParent = NULL);	// standard constructor
	~AccountDlg();
#ifndef _GLOBAL_ACCOUNT_MINI
	enum { IDD = IDD_ACCOUNT };
#else
	enum { IDD = IDD_ACCOUNT_CUSTOM };
#endif


protected:
#ifdef _GLOBAL_ACCOUNT_MINI
	HICON m_hIcon;
#endif

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
#ifndef _GLOBAL_CUSTOM
	afx_msg void OnNMClickSyslinkSipServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkSipProxy(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkUsername(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkDomain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkAuthID(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkPassword(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkName(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkEncryption(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkTransport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkPublicAddress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkLocalPort(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkPublishPresence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkStunServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkIce(NMHDR *pNMHDR, LRESULT *pResult);
#endif
	afx_msg void OnNMClickSyslinkDisplayPasswod(NMHDR *pNMHDR, LRESULT *pResult);
#ifdef _GLOBAL_ACCOUNT_MINI
	afx_msg void OnPaint();
#endif
};
