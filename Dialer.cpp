#include "StdAfx.h"
#include "Dialer.h"
#include "global.h"
#include "settings.h"
#include "microsipDlg.h"
#include "microsip.h"

static CmicrosipDlg *microsipDlg;

Dialer::Dialer(CWnd* pParent /*=NULL*/)
: CDialog(Dialer::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();
	Create (IDD, pParent);
}

Dialer::~Dialer(void)
{
}

BOOL Dialer::OnInitDialog()
{
	CDialog::OnInitDialog();

	TranslateDialog(this->m_hWnd);

	CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
	DialedLoad(combobox);
	combobox->SetWindowPos(NULL,0,0,combobox->GetDroppedWidth(),400,SWP_NOZORDER|SWP_NOMOVE);

	CFont* font = this->GetFont();
	LOGFONT lf;
	font->GetLogFont(&lf);
	lf.lfHeight = 18;
	m_font.CreateFontIndirect(&lf);

	lf.lfHeight = 17;
	m_font_combo.CreateFontIndirect(&lf);
	combobox->SetFont(&m_font_combo);

	GetDlgItem(IDC_KEY_1)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_2)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_3)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_4)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_5)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_6)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_7)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_8)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_9)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_0)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_STAR)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_GRATE)->SetFont(&m_font);
	GetDlgItem(IDC_KEY_PLUS)->SetFont(&m_font);
	GetDlgItem(IDC_CLEAR)->SetFont(&m_font);
	GetDlgItem(IDC_DELETE)->SetFont(&m_font);

	return TRUE;
}

void Dialer::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(Dialer, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CALL, OnBnClickedCall)
	ON_CBN_EDITCHANGE(IDC_NUMBER, &Dialer::OnCbnEditchangeComboAddr)
	ON_CBN_SELCHANGE(IDC_NUMBER, &Dialer::OnCbnSelchangeComboAddr)
	ON_BN_CLICKED(IDC_KEY_1, &Dialer::OnBnClickedKey1)
	ON_BN_CLICKED(IDC_KEY_2, &Dialer::OnBnClickedKey2)
	ON_BN_CLICKED(IDC_KEY_3, &Dialer::OnBnClickedKey3)
	ON_BN_CLICKED(IDC_KEY_4, &Dialer::OnBnClickedKey4)
	ON_BN_CLICKED(IDC_KEY_5, &Dialer::OnBnClickedKey5)
	ON_BN_CLICKED(IDC_KEY_6, &Dialer::OnBnClickedKey6)
	ON_BN_CLICKED(IDC_KEY_7, &Dialer::OnBnClickedKey7)
	ON_BN_CLICKED(IDC_KEY_8, &Dialer::OnBnClickedKey8)
	ON_BN_CLICKED(IDC_KEY_9, &Dialer::OnBnClickedKey9)
	ON_BN_CLICKED(IDC_KEY_STAR, &Dialer::OnBnClickedKeyStar)
	ON_BN_CLICKED(IDC_KEY_0, &Dialer::OnBnClickedKey0)
	ON_BN_CLICKED(IDC_KEY_GRATE, &Dialer::OnBnClickedKeyGrate)
	ON_BN_CLICKED(IDC_KEY_PLUS, &Dialer::OnBnClickedKeyPlus)
	ON_BN_CLICKED(IDC_CLEAR, &Dialer::OnBnClickedClear)
	ON_BN_CLICKED(IDC_DELETE, &Dialer::OnBnClickedDelete)
END_MESSAGE_MAP()

BOOL Dialer::PreTranslateMessage(MSG* pMsg)
{
	BOOL catched = FALSE;
	BOOL isEdit = FALSE;
	if (pMsg->message == WM_CHAR || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
		CWnd* hWndEdit = FindWindowEx(combobox->m_hWnd,0,TEXT("EDIT"),0 );
		isEdit = hWndEdit == GetFocus();
	}
	if (pMsg->message == WM_CHAR)
	{
		if (pMsg->wParam == 48)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_0));
				OnBnClickedKey0();
				catched = TRUE;
			} else {
				DTMF("0");
			}
		} else if (pMsg->wParam == 49)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_1));
				OnBnClickedKey1();
				catched = TRUE;
			} else {
				DTMF("1");
			}
		} else if (pMsg->wParam == 50)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_2));
				OnBnClickedKey2();
				catched = TRUE;
			} else {
				DTMF("2");
			}
		} else if (pMsg->wParam == 51)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_3));
				OnBnClickedKey3();
				catched = TRUE;
			} else {
				DTMF("3");
			}
		} else if (pMsg->wParam == 52)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_4));
				OnBnClickedKey4();
				catched = TRUE;
			} else {
				DTMF("4");
			}
		} else if (pMsg->wParam == 53)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_5));
				OnBnClickedKey5();
				catched = TRUE;
			} else {
				DTMF("5");
			}
		} else if (pMsg->wParam == 54)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_6));
				OnBnClickedKey6();
				catched = TRUE;
			} else {
				DTMF("6");
			}
		} else if (pMsg->wParam == 55)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_7));
				OnBnClickedKey7();
				catched = TRUE;
			} else {
				DTMF("7");
			}
		} else if (pMsg->wParam == 56)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_8));
				OnBnClickedKey8();
				catched = TRUE;
			} else {
				DTMF("8");
			}
		} else if (pMsg->wParam == 57)
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_9));
				OnBnClickedKey9();
				catched = TRUE;
			} else {
				DTMF("9");
			}
		} else if (pMsg->wParam == 35 || pMsg->wParam == 47 )
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_GRATE));
				OnBnClickedKeyGrate();
				catched = TRUE;
			} else {
				DTMF("#");
			}
		} else if (pMsg->wParam == 42 )
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_STAR));
				OnBnClickedKeyStar();
				catched = TRUE;
			} else {
				DTMF("*");
			}
		} else if (pMsg->wParam == 43 )
		{
			if (!isEdit) {
				GotoDlgCtrl(GetDlgItem(IDC_KEY_PLUS));
				OnBnClickedKeyPlus();
				catched = TRUE;
			}
		} else if (pMsg->wParam == 8 || pMsg->wParam == 45 )
		{
			if (!isEdit)
			{
				GotoDlgCtrl(GetDlgItem(IDC_DELETE));
				OnBnClickedDelete();
				catched = TRUE;
			}
		} else if (pMsg->wParam == 46 )
		{
			if (!isEdit)
			{
				Input(".", TRUE);
				catched = TRUE;
			}
		}
	} else if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnBnClickedOk();
			catched = TRUE;
		} else if (pMsg->wParam == VK_ESCAPE)
		{
			if (!isEdit) { GotoDlgCtrl(GetDlgItem(IDC_CLEAR)); }
			OnBnClickedClear();
			catched = TRUE;
		}
	}
	if (!catched)
	{
		return CDialog::PreTranslateMessage(pMsg);
	} else {
		return TRUE;
	}
}

void Dialer::OnBnClickedOk()
{
	GotoDlgCtrl(GetDlgItem(IDC_CALL));
	OnBnClickedCall();
}

void Dialer::OnBnClickedCancel()
{
}

void Dialer::OnBnClickedCall()
{
	if (accountSettings.singleMode && pjsua_call_get_count())
	{
		MessagesContact*  messagesContact = microsipDlg->messagesDlg->GetMessageContact();
		if (messagesContact && messagesContact->callId != -1 )
		{
			pjsua_call_hangup(messagesContact->callId, 0, NULL, NULL);
		} else 
		{
			pjsua_call_hangup_all();
		}
	} else 
	{
		CString number;
		CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
		combobox->GetWindowText(number);

		if (number!="") {

			number.Trim();

			BOOL isDigits = TRUE;
			for (int i=0;i<number.GetLength();i++)
			{
				if ( (number[i]>'9' || number[i]<'0') && number[i]!='.' && number[i]!='-' && number[i]!='(' && number[i]!=')' && number[i]!=' ' && number[0]!='+')
				{
					isDigits = FALSE;
					break;
				}
			}
			CString numberFormated = number;
			if (isDigits) {
				numberFormated.Remove('.');
				numberFormated.Remove('-');
				numberFormated.Remove('(');
				numberFormated.Remove(')');
				numberFormated.Remove(' ');
			}

			numberFormated = microsipDlg->GetSIPURI(numberFormated);
			pj_status_t pj_status = pjsua_verify_sip_url(numberFormated);
			if (pj_status==PJ_SUCCESS)
			{
				int pos = combobox->FindStringExact(-1,number);
				if (pos==CB_ERR || pos>0)
				{
					if (pos>0)
					{
						combobox->DeleteString(pos);
					} else if (combobox->GetCount()>=10)
					{
						combobox->DeleteString(combobox->GetCount()-1);
					}
					combobox->InsertString(0,number);
					combobox->SetCurSel(0);
				}
				DialedSave(combobox);

				microsipDlg->messagesDlg->AddTab(numberFormated, "", TRUE, NULL, accountSettings.singleMode);
				microsipDlg->messagesDlg->Call();
			} else 
			{
				microsipDlg->ShowErrorMessage(pj_status);
			}
		}
	}
}

void Dialer::DTMF(CString digits)
{
	BOOL simulate = TRUE;
	MessagesContact*  messagesContact = microsipDlg->messagesDlg->GetMessageContact();
	if (messagesContact && messagesContact->callId != -1 )
	{
		pjsua_call_info call_info;
		pjsua_call_get_info(messagesContact->callId, &call_info);
		if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE)
		{
			pj_str_t pj_digits = pj_str ( digits.GetBuffer() );
			if (pjsua_call_dial_dtmf(messagesContact->callId, &pj_digits) != PJ_SUCCESS) {
				simulate = !call_play_digit(messagesContact->callId, digits.GetBuffer());
			}
		}
	}
	if (simulate && accountSettings.localDTMF) {
		call_play_digit(-1, digits.GetBuffer());
	}
}

void Dialer::Input(CString digits, BOOL disableDTMF)
{
	if (!disableDTMF)
	{
		DTMF(digits);
	}
	CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
	CEdit* edit = (CEdit*)FindWindowEx(combobox->m_hWnd,0,TEXT("EDIT"),0 );
	//int pos = edit->CharFromPos(edit->GetCaretPos());
	/*
	CString s;
	s.Format("%d", pos);
	AfxMessageBox(s);
	*/

	int nLength = edit->GetWindowTextLength();
	edit->SetSel(nLength,nLength);
	edit->ReplaceSel(digits);


	//edit->SetSel(HIWORD(pos), HIWORD(pos));

	//hWndEdit->replaceS
	//int nLength = hWndEdit->GetWindowTextLength();
	//::SendMessage(hWndEdit->m_hWnd , EM_SETSEL, (WPARAM)nLength, (LPARAM)nLength);
	/*
	CEdit e;
	e.ReplaceSel(

	CPoint caret = hWndEdit->GetCaretPos();
	CString s;
	s.Format("%d %d", caret.x,
	caret.y);
	AfxMessageBox(s);


	::SendMessage(hWndEdit->m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)digits.GetBuffer());
	*/


}

void Dialer::DialedLoad(CComboBox *combobox)
{
	CString key;
	CString val;
	char * ptr = val.GetBuffer(255);
	int i=0;
	while (TRUE) {
		key.Format("%d",i);
		if (GetPrivateProfileString("Dialed", key, NULL, ptr, 256, accountSettings.iniFile)) {
			combobox->AddString(ptr);
		} else {
			break;
		}
		i++;
	}
}

void Dialer::DialedSave(CComboBox *combobox)
{
	CString key;
	CString val;
	WritePrivateProfileString("Dialed", NULL, NULL, accountSettings.iniFile);
	for (int i=0;i < combobox->GetCount();i++)
	{
		int n = combobox->GetLBTextLen( i );
		combobox->GetLBText( i, val.GetBuffer(n) );
		val.ReleaseBuffer();

		key.Format("%d",i);
		WritePrivateProfileString("Dialed", key, val, accountSettings.iniFile);
	}
}


void Dialer::OnBnClickedKey1()
{
	Input("1");
}

void Dialer::OnBnClickedKey2()
{
	Input("2");
}

void Dialer::OnBnClickedKey3()
{
	Input("3");
}

void Dialer::OnBnClickedKey4()
{
	Input("4");
}

void Dialer::OnBnClickedKey5()
{
	Input("5");
}

void Dialer::OnBnClickedKey6()
{
	Input("6");
}

void Dialer::OnBnClickedKey7()
{
	Input("7");
}

void Dialer::OnBnClickedKey8()
{
	Input("8");
}

void Dialer::OnBnClickedKey9()
{
	Input("9");
}

void Dialer::OnBnClickedKeyStar()
{
	Input("*");
}

void Dialer::OnBnClickedKey0()
{
	Input("0");
}

void Dialer::OnBnClickedKeyGrate()
{
	Input("#");
}

void Dialer::OnBnClickedKeyPlus()
{
	Input("+", TRUE);
}

void Dialer::OnBnClickedClear()
{
	GetDlgItem(IDC_NUMBER)->SetWindowText(NULL);
	UpdateCallButton();
}

void Dialer::OnBnClickedDelete()
{
	CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
	CWnd* hWndEdit = FindWindowEx(combobox->m_hWnd,0,TEXT("EDIT"),0 );
	int nLength = hWndEdit->GetWindowTextLength();
	::SendMessage(hWndEdit->m_hWnd , EM_SETSEL, (WPARAM)(nLength-1), (LPARAM)nLength);
	char c[1]="";
	::SendMessage(hWndEdit->m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)c);
}

void Dialer::UpdateCallButton(BOOL forse, int callsCount)
{
	int len;
	if (!forse)
	{
		CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
		len = combobox->GetWindowTextLength();
	} else
	{
		len = 1;
	}
	if (accountSettings.singleMode)
	{
		if (callsCount == -1)
		{
			callsCount = pjsua_call_get_count();
		}
		GetDlgItem(IDC_CALL)->SetWindowText(Translate(callsCount?"End":"Call"));
		((CButton*)GetDlgItem(IDC_CALL))->EnableWindow(callsCount||len?TRUE:FALSE);
	} else
	{
		((CButton*)GetDlgItem(IDC_CALL))->EnableWindow(len?TRUE:FALSE);
	}
}

void Dialer::SetNumber(CString  number)
{
	CComboBox *combobox= (CComboBox*)GetDlgItem(IDC_NUMBER);
	CString old;
	combobox->GetWindowText(old);
	if (number.Find(old)!=0) {
		combobox->SetWindowText(number);
	}
	UpdateCallButton();
}

void Dialer::OnCbnEditchangeComboAddr()
{
	UpdateCallButton();
}

void Dialer::OnCbnSelchangeComboAddr()
{	
	UpdateCallButton(TRUE);
}
