#include "StdAfx.h"
#include "AccountDlg.h"
#include "microsipDlg.h"
#include "settings.h"

static CmicrosipDlg *microsipDlg;

AccountDlg::AccountDlg(CWnd* pParent /*=NULL*/)
: CDialog(AccountDlg::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();	
	Create (IDD, pParent);
}

AccountDlg::~AccountDlg(void)
{
	microsipDlg->accountDlg = NULL;
}


BOOL AccountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	TranslateDialog(this->m_hWnd);

	CEdit* edit;

#ifndef _GLOBAL_ACCOUNT_MINI

	CComboBox *combobox;
	int i;

	edit = (CEdit*)GetDlgItem(IDC_EDIT_SERVER);
	edit->SetWindowText(accountSettings.server);

	edit = (CEdit*)GetDlgItem(IDC_EDIT_PROXY);
	edit->SetWindowText(accountSettings.proxy);

	combobox= (CComboBox*)GetDlgItem(IDC_SRTP);
	combobox->AddString(Translate("Disabled"));
	combobox->AddString(Translate("Optional"));
	combobox->AddString(Translate("Mandatory"));
	if (accountSettings.srtp=="optional")
	{
		i=1;
	} else if (accountSettings.srtp=="mandatory")
	{
		i=2;
	} else
	{
		i=0;
	}
	combobox->SetCurSel(i);

	combobox= (CComboBox*)GetDlgItem(IDC_TRANSPORT);
	combobox->AddString(Translate("Auto"));
	combobox->AddString("UDP");
	combobox->AddString("TCP");
	combobox->AddString("TLS");
	if (accountSettings.transport=="udp")
	{
		i=1;
	} else if (accountSettings.transport=="tcp")
	{
		i=2;
	} else if (accountSettings.transport=="tls")
	{
		i=3;
	} else
	{
		i=0;
	}
	combobox->SetCurSel(i);

	combobox= (CComboBox*)GetDlgItem(IDC_PUBLIC_ADDR);
	combobox->AddString(Translate("Auto"));
	PHOSTENT hostinfo;
	char name[255];
	char *ip;
	if( gethostname ( name, sizeof(name) ) == 0 )
	{
		if( (hostinfo = gethostbyname(name)) != NULL )
		{
			int nCount = 0;
			while(hostinfo->h_addr_list[nCount])
			{
				ip = inet_ntoa(*(
				struct in_addr *)hostinfo->h_addr_list[nCount]);
				combobox->AddString(ip);
				nCount++;
			}
		}
	}
	if (!accountSettings.publicAddr.GetLength())
	{
		combobox->SetCurSel(0);
	} else {
		combobox->SetWindowText(accountSettings.publicAddr);
	}

	combobox= (CComboBox*)GetDlgItem(IDC_LISTEN_PORT);
	combobox->AddString(Translate("Auto"));
	if (!accountSettings.listenPort.GetLength())
	{
		combobox->SetCurSel(0);
	} else {
		combobox->SetWindowText(accountSettings.listenPort);
	}

	((CButton*)GetDlgItem(IDC_PUBLISH))->SetCheck(accountSettings.publish);

	edit = (CEdit*)GetDlgItem(IDC_STUN);
	edit->SetWindowText(accountSettings.stun);

	((CButton*)GetDlgItem(IDC_ICE))->SetCheck(accountSettings.ice);

	edit = (CEdit*)GetDlgItem(IDC_EDIT_DOMAIN);
	edit->SetWindowText(accountSettings.domain);

	edit = (CEdit*)GetDlgItem(IDC_EDIT_AUTHID);
	edit->SetWindowText(accountSettings.authID);

#else
	m_hIcon = (HICON)LoadImage(
                AfxGetInstanceHandle(),
                MAKEINTRESOURCE(IDI_LOGO),
                IMAGE_ICON, 0, 0, LR_SHARED );
#endif

	edit = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
	edit->SetWindowText(accountSettings.username);

	edit = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
	edit->SetWindowText(accountSettings.password);

#ifndef _GLOBAL_NO_NAME
	edit = (CEdit*)GetDlgItem(IDC_EDIT_DISPLAYNAME);
	edit->SetWindowText(accountSettings.displayName);
#endif

	return TRUE;
}

void AccountDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(AccountDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &AccountDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &AccountDlg::OnBnClickedOk)
#ifndef _GLOBAL_CUSTOM
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SIP_SERVER, &AccountDlg::OnNMClickSyslinkSipServer)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SIP_PROXY, &AccountDlg::OnNMClickSyslinkSipProxy)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_USERNAME, &AccountDlg::OnNMClickSyslinkUsername)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DOMAIN, &AccountDlg::OnNMClickSyslinkDomain)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_AUTHID, &AccountDlg::OnNMClickSyslinkAuthID)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_PASSWORD, &AccountDlg::OnNMClickSyslinkPassword)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_NAME, &AccountDlg::OnNMClickSyslinkName)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_ENCRYPTION, &AccountDlg::OnNMClickSyslinkEncryption)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TRANSPORT, &AccountDlg::OnNMClickSyslinkTransport)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_PUBLIC_ADDRESS, &AccountDlg::OnNMClickSyslinkPublicAddress)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_LOCAL_PORT, &AccountDlg::OnNMClickSyslinkLocalPort)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_PUBLISH_PRESENCE, &AccountDlg::OnNMClickSyslinkPublishPresence)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_STUN_SERVER, &AccountDlg::OnNMClickSyslinkStunServer)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_ICE, &AccountDlg::OnNMClickSyslinkIce)
#else
	ON_WM_PAINT()
#endif
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISPLAY_PASSWORD, &AccountDlg::OnNMClickSyslinkDisplayPasswod)
END_MESSAGE_MAP()


void AccountDlg::OnClose() 
{
	DestroyWindow();
}

void AccountDlg::OnBnClickedCancel()
{
	OnClose();
}

void AccountDlg::OnBnClickedOk()
{
	CEdit* edit;
	CString str;

#ifndef _GLOBAL_ACCOUNT_MINI

	CComboBox *combobox;
	int i;

	edit = (CEdit*)GetDlgItem(IDC_EDIT_SERVER);
	edit->GetWindowText(str);
	accountSettings.server=str.Trim();

	edit = (CEdit*)GetDlgItem(IDC_EDIT_PROXY);
	edit->GetWindowText(str);
	accountSettings.proxy=str.Trim();

	combobox= (CComboBox*)GetDlgItem(IDC_SRTP);
	i = combobox->GetCurSel();
	switch (i) {
		case 1:
			accountSettings.srtp="optional";
			break;
		case 2:
			accountSettings.srtp="mandatory";
			break;
		default:
			accountSettings.srtp="";
	}

	combobox= (CComboBox*)GetDlgItem(IDC_TRANSPORT);
	i = combobox->GetCurSel();
	switch (i) {
		case 1:
			accountSettings.transport="udp";
			break;
		case 2:
			accountSettings.transport="tcp";
			break;
		case 3:
			accountSettings.transport="tls";
			break;
		default:
			accountSettings.transport="";
	}

	combobox= (CComboBox*)GetDlgItem(IDC_PUBLIC_ADDR);
	i = combobox->GetCurSel();
	combobox->GetWindowText(accountSettings.publicAddr);
	if (accountSettings.publicAddr==Translate("Auto"))
	{
		accountSettings.publicAddr = "";
	}

	combobox= (CComboBox*)GetDlgItem(IDC_LISTEN_PORT);
	i = combobox->GetCurSel();
	combobox->GetWindowText(accountSettings.listenPort);
	int port = atoi(accountSettings.listenPort.GetBuffer());
	if (port<=0 || port>65535 || accountSettings.listenPort==Translate("Auto"))
	{
		accountSettings.listenPort = "";
	}

	accountSettings.publish = ((CButton*)GetDlgItem(IDC_PUBLISH))->GetCheck();

	edit = (CEdit*)GetDlgItem(IDC_STUN);
	edit->GetWindowText(str);
	accountSettings.stun=str.Trim();

	accountSettings.ice = ((CButton*)GetDlgItem(IDC_ICE))->GetCheck();

	edit = (CEdit*)GetDlgItem(IDC_EDIT_DOMAIN);
	edit->GetWindowText(str);
	accountSettings.domain=str.Trim();

	edit = (CEdit*)GetDlgItem(IDC_EDIT_AUTHID);
	edit->GetWindowText(str);
	accountSettings.authID=str.Trim();

#endif

	edit = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
	edit->GetWindowText(str);
	accountSettings.username=str.Trim();

	edit = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
	edit->GetWindowText(str);
	accountSettings.password=str.Trim();

#ifndef _GLOBAL_NO_NAME
	edit = (CEdit*)GetDlgItem(IDC_EDIT_DISPLAYNAME);
#else
	edit = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
#endif
	edit->GetWindowText(str);
	accountSettings.displayName=str.Trim();

	accountSettings.Save();

	this->ShowWindow(SW_HIDE);

	microsipDlg->PJDestroy();

	microsipDlg->PJCreate();
	microsipDlg->PJAccountAdd();

	OnClose();
}

#ifndef _GLOBAL_CUSTOM

void AccountDlg::OnNMClickSyslinkSipServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("sipServer");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkSipProxy(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("sipProxy");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkUsername(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("username");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkDomain(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("domain");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkAuthID(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("login");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkPassword(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("password");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkName(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("name");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkEncryption(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("encryption");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkTransport(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("transport");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkPublicAddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("publicAddress");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkLocalPort(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("localPort");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkPublishPresence(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("publishPresence");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkStunServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("stunServer");
	*pResult = 0;
}

void AccountDlg::OnNMClickSyslinkIce(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("ice");
	*pResult = 0;
}

#endif

void AccountDlg::OnNMClickSyslinkDisplayPasswod(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetDlgItem(IDC_SYSLINK_DISPLAY_PASSWORD)->ShowWindow(SW_HIDE);
	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
	edit->SetPasswordChar(0);
	edit->Invalidate();
	edit->SetFocus();
	int nLength = edit->GetWindowTextLength();
	edit->SetSel(nLength,nLength);
	*pResult = 0;
}

#ifdef _GLOBAL_ACCOUNT_MINI
void AccountDlg::OnPaint() 
{
	if (!IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		DrawIconEx(dc, 10, 10, m_hIcon, 0, 0, NULL, NULL, DI_MASK | DI_IMAGE);
	}
	else
	{
		CDialog::OnPaint();
	}
}
#endif
