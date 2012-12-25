#pragma once

#include "resource.h"
#include "AddDlg.h"

class Contacts :
	public CDialog
{
public:
	Contacts(CWnd* pParent = NULL);	// standard constructor
	~Contacts();
	enum { IDD = IDD_CONTACTS };
	AddDlg* addDlg;

	void ContactAdd(CString number, CString name, BOOL presence, BOOL save = FALSE);
	void ContactDelete(int i);
	void UpdateCallButton();
	CString GetNameByNumber(CString number);

private:
	CImageList* imageList;
	void ContactsLoad();
	void ContactsSave();
	void ContactDecode(CString str, CString &number, CString &name, BOOL &presence);
	void MessageDlgOpen(BOOL isCall = FALSE);

protected:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMenuCall(); 
	afx_msg void OnMenuChat();
	afx_msg void OnMenuAdd(); 
	afx_msg void OnMenuEdit(); 
	afx_msg void OnMenuDelete(); 
	afx_msg void OnNMRClickContacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkContacts(NMHDR *pNMHDR, LRESULT *pResult);
};
