#pragma once

// диалоговое окно addDlg

class AddDlg : public CDialog
{

public:
	AddDlg(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~AddDlg();
	enum { IDD = IDD_ADD };

	int listIndex;

protected:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
