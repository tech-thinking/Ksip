#pragma once

#include "resource.h"

class Dialer :
	public CDialog
{
	CFont m_font;
	CFont m_font_combo;
public:
	Dialer(CWnd* pParent = NULL);	// standard constructor
	~Dialer();
	enum { IDD = IDD_DIALER };

	void Input(CString digits, BOOL disableDTMF = FALSE);
	void DTMF(CString digits);
	void DialedLoad(CComboBox *combobox);
	void DialedSave(CComboBox *combobox);
	void SetNumber(CString  number);
	void UpdateCallButton(BOOL forse = FALSE, int callsCount = -1);

protected:

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedCall();
	
	afx_msg void OnCbnEditchangeComboAddr();
	afx_msg void OnCbnSelchangeComboAddr();
	afx_msg void OnBnClickedKey1();
	afx_msg void OnBnClickedKey2();
	afx_msg void OnBnClickedKey3();
	afx_msg void OnBnClickedKey4();
	afx_msg void OnBnClickedKey5();
	afx_msg void OnBnClickedKey6();
	afx_msg void OnBnClickedKey7();
	afx_msg void OnBnClickedKey8();
	afx_msg void OnBnClickedKey9();
	afx_msg void OnBnClickedKeyStar();
	afx_msg void OnBnClickedKey0();
	afx_msg void OnBnClickedKeyGrate();
	afx_msg void OnBnClickedKeyPlus();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedDelete();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
