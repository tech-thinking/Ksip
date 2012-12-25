#include "StdAfx.h"
#include "Contacts.h"
#include "microsip.h"
#include "global.h"
#include "settings.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "microsipDlg.h"

static CmicrosipDlg *microsipDlg;

Contacts::Contacts(CWnd* pParent /*=NULL*/)
: CDialog(Contacts::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();	
	Create (IDD, pParent);
}

Contacts::~Contacts(void)
{
}

BOOL Contacts::OnInitDialog()
{
	CDialog::OnInitDialog();

	TranslateDialog(this->m_hWnd);
	
	addDlg = new AddDlg(this);
	imageList = new CImageList();
	imageList->Create(16,16,ILC_COLOR32,3,3);
	imageList->SetBkColor(RGB(255, 255, 255));
	imageList->Add(theApp.LoadIcon(IDI_UNKNOWN));
	imageList->Add(theApp.LoadIcon(IDI_OFFLINE));
	imageList->Add(theApp.LoadIcon(IDI_AWAY));
	imageList->Add(theApp.LoadIcon(IDI_ONLINE));
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	list->SetImageList(imageList,LVSIL_SMALL);

	ContactsLoad();

	return TRUE;
}

void Contacts::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	microsipDlg->pageContacts=NULL;
	delete imageList;
	delete this;
}

BEGIN_MESSAGE_MAP(Contacts, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_CALL,OnMenuCall)
	ON_COMMAND(ID_CHAT,OnMenuChat)
	ON_COMMAND(ID_ADD,OnMenuAdd)
	ON_COMMAND(ID_EDIT,OnMenuEdit)
	ON_COMMAND(ID_DELETE,OnMenuDelete)
	ON_NOTIFY(NM_RCLICK, IDC_CONTACTS, &Contacts::OnNMRClickContacts)
	ON_NOTIFY(NM_DBLCLK, IDC_CONTACTS, &Contacts::OnNMDblclkContacts)
END_MESSAGE_MAP()


void Contacts::OnBnClickedOk()
{
	MessageDlgOpen();
}

void Contacts::OnBnClickedCancel()
{
}

void Contacts::OnNMRClickContacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint point;
	GetCursorPos(&point);
	CMenu menu;
	menu.LoadMenu(IDR_MENU_CONTACT);
	CMenu* tracker = menu.GetSubMenu(0);
	TranslateMenu(tracker->m_hMenu);
	if ( pNMItemActivate->iItem != -1 ) {
		CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
		list->SetItemState(pNMItemActivate->iItem, LVIS_SELECTED, LVIS_SELECTED);
		tracker->EnableMenuItem(ID_CALL, FALSE);
		tracker->EnableMenuItem(ID_CHAT, FALSE);
		tracker->EnableMenuItem(ID_EDIT, FALSE);
		tracker->EnableMenuItem(ID_DELETE, FALSE);
	} else {
		tracker->EnableMenuItem(ID_CALL, TRUE);
		tracker->EnableMenuItem(ID_CHAT, TRUE);
		tracker->EnableMenuItem(ID_EDIT, TRUE);
		tracker->EnableMenuItem(ID_DELETE, TRUE);
	}
#ifdef _GLOBAL_NO_MESSAGING
	tracker->RemoveMenu(ID_CHAT,MF_BYCOMMAND);
#endif
	tracker->TrackPopupMenu( 0, point.x, point.y, this );
	*pResult = 0;
}

void Contacts::MessageDlgOpen(BOOL isCall)
{
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	POSITION pos = list->GetFirstSelectedItemPosition();
	if (pos)
	{
		int i = list->GetNextSelectedItem(pos);
		Contact *pContact = (Contact *) list->GetItemData(i);
		microsipDlg->messagesDlg->AddTab(microsipDlg->GetSIPURI(pContact->number), pContact->name, TRUE, NULL, isCall && accountSettings.singleMode);
		if (isCall)
		{
			microsipDlg->messagesDlg->Call();
		}
	}
}

void Contacts::OnNMDblclkContacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem!=-1) {
		MessageDlgOpen(accountSettings.singleMode);
	}
	*pResult = 0;
}

void Contacts::OnMenuCall()
{
	MessageDlgOpen(TRUE);
}

void Contacts::OnMenuChat()
{
	MessageDlgOpen();
}

void Contacts::OnMenuAdd()
{
if (!addDlg->IsWindowVisible()) {
		addDlg->ShowWindow(SW_SHOW);
	} else {
		addDlg->SetForegroundWindow();
	}
	addDlg->listIndex = -1;
	addDlg->GetDlgItem(IDC_EDIT_NUMBER)->SetWindowText(NULL);
	addDlg->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(NULL);
#ifndef _GLOBAL_ACCOUNT_MINI
	((CButton *)addDlg->GetDlgItem(IDC_PRESENCE))->SetCheck(0);
#endif
}

void Contacts::OnMenuEdit()
{
	OnMenuAdd();
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	POSITION pos = list->GetFirstSelectedItemPosition();
	int i = list->GetNextSelectedItem(pos);
	addDlg->listIndex = i;

	Contact *pContact = (Contact *) list->GetItemData(i);
	addDlg->GetDlgItem(IDC_EDIT_NUMBER)->SetWindowText(pContact->number);
	addDlg->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(pContact->name);
#ifndef _GLOBAL_ACCOUNT_MINI
	((CButton *)addDlg->GetDlgItem(IDC_PRESENCE))->SetCheck(pContact->presence);
#endif
}

void Contacts::OnMenuDelete()
{
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	POSITION pos = list->GetFirstSelectedItemPosition();
	ContactDelete(list->GetNextSelectedItem(pos));
}

void Contacts::ContactDelete(int i)
{
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	Contact *pContact = (Contact *) list->GetItemData(i);
	microsipDlg->PresenceUnsubsribeOne(pContact);
	delete pContact;
	list->DeleteItem(i);
	ContactsSave();
}

void Contacts::ContactsSave()
{
	CString key;
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	WritePrivateProfileString("Contacts", NULL, NULL, accountSettings.iniFile);
	int count = list->GetItemCount();
	for (int i=0;i<count;i++)
	{
		Contact *pContact = (Contact *) list->GetItemData(i);
		CString contactData;
		contactData.Format("%s;%s;%d", pContact->number, pContact->name, pContact->presence);
		key.Format("%d",i);
		WritePrivateProfileString("Contacts", key, contactData, accountSettings.iniFile);
	}
}

void Contacts::ContactAdd(CString number, CString name, BOOL presence, BOOL save)
{
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);
	Contact *pContact =  new Contact();
	pContact->number = number;
	pContact->name = name;
	pContact->presence = presence;
	pContact->presenceTime = 0;
	int i = list->InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE,0,name,0,0,0,(LPARAM)pContact);
	if (save) {
		ContactsSave();
		if (pContact->presence)
		{
			microsipDlg->PresenceSubsribeOne(pContact);
		}
	}	
}

void Contacts::ContactsLoad()
{
	CString key;
	CString val;
	char * ptr = val.GetBuffer(255);
	int i=0;
	while (TRUE) {
		key.Format("%d",i);
		if (GetPrivateProfileString("Contacts", key, NULL, ptr, 256, accountSettings.iniFile)) {
			CString number;
			CString name;
			BOOL presence;
			ContactDecode(ptr, number, name, presence);
			ContactAdd(number, name, presence);
		} else {
			break;
		}
		i++;
	}
}

void Contacts::ContactDecode(CString str, CString &number, CString &name, BOOL &presence)
{
	CString rab;
	int begin;
	int end;
	begin = 0;
	end = str.Find(';', begin);
	if (end != -1)
	{
		number=str.Mid(begin, end-begin);
		begin = end + 1;
		end = str.Find(';', begin);
		if (end != -1)
		{
			name=str.Mid(begin, end-begin);
			begin = end + 1;
			end = str.Find(';', begin);
			if (end != -1)
			{
				rab=str.Mid(begin, end-begin);
				presence = rab == "1";
			} else 
			{
				rab=str.Mid(begin);
				presence = rab == "1";
			}
		} else 
		{
			name = str.Mid(begin);
			presence = FALSE;
		}
	} else 
	{
		number=str;
		name = number;
		presence = FALSE;
	}
}

CString Contacts::GetNameByNumber(CString number)
{
	CString name;
	CListCtrl *list= (CListCtrl*)GetDlgItem(IDC_CONTACTS);

	CString sipURI = microsipDlg->GetSIPURI(number);
	int n = list->GetItemCount();
	for (int i=0; i<n; i++) {
		Contact* pContact = (Contact *) list->GetItemData(i);
		if (microsipDlg->GetSIPURI(pContact->number) == sipURI)
		{
			name = pContact->name;
			break;
		}
	}
	return name;
}