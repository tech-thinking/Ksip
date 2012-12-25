#include "StdAfx.h"
#include "SettingsDlg.h"
#include "microsipDlg.h"
#include "settings.h"
#include "Preview.h"

static BOOL prev;

static CmicrosipDlg *microsipDlg;

SettingsDlg::SettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(SettingsDlg::IDD, pParent)
{
	microsipDlg = (CmicrosipDlg* ) AfxGetMainWnd();	
	Create (IDD, pParent);
	prev = FALSE;
}

SettingsDlg::~SettingsDlg(void)
{
	microsipDlg->settingsDlg = NULL;
}

BOOL SettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TranslateDialog(this->m_hWnd);
#ifndef _GLOBAL_NO_AUTO
	((CButton*)GetDlgItem(IDC_AUTO_ANSWER))->SetCheck(accountSettings.autoAnswer);
#endif
	((CButton*)GetDlgItem(IDC_LOCAL_DTMF))->SetCheck(accountSettings.localDTMF);
#ifndef _GLOBAL_SINGLE_MODE
	((CButton*)GetDlgItem(IDC_SINGLE_MODE))->SetCheck(accountSettings.singleMode);
#endif
#ifndef _GLOBAL_NO_LOG
	((CButton*)GetDlgItem(IDC_ENABLE_LOG))->SetCheck(accountSettings.enableLog);
#endif
#ifndef _GLOBAL_NO_VAD
	((CButton*)GetDlgItem(IDC_DISABLE_VAD))->SetCheck(accountSettings.disableVAD);
#endif
#ifndef _GLOBAL_NO_RINGING_SOUND
	GetDlgItem(IDC_RINGING_SOUND)->SetWindowText(accountSettings.ringingSound);
#endif
	CComboBox *combobox;
	unsigned count;


	pjmedia_aud_dev_info aud_dev_info[64];
	count = 64;
	pjsua_enum_aud_devs(aud_dev_info, &count);

	combobox= (CComboBox*)GetDlgItem(IDC_MICROPHONE);
	combobox->AddString(Translate("Default"));
	combobox->SetCurSel(0);
	for (unsigned i=0;i<count;i++)
	{
		if (aud_dev_info[i].input_count) {
			combobox->AddString(aud_dev_info[i].name);
			if (!accountSettings.audioInputDevice.Compare(aud_dev_info[i].name))
			{
				combobox->SetCurSel(combobox->GetCount()-1);
			}
		}
	}
	combobox= (CComboBox*)GetDlgItem(IDC_SPEAKERS);
	combobox->AddString(Translate("Default"));
	combobox->SetCurSel(0);
	for (unsigned i=0;i<count;i++)
	{
		if (aud_dev_info[i].output_count) {
			combobox->AddString(aud_dev_info[i].name);
			if (!accountSettings.audioOutputDevice.Compare(aud_dev_info[i].name))
			{
				combobox->SetCurSel(combobox->GetCount()-1);
			}
		}
	}

	pjsua_codec_info codec_info[64];
	CListBox *listbox;
	CListBox *listbox2;

	listbox = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS_ALL);
	listbox2 = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS);
	count = 64;
	pjsua_enum_codecs(codec_info, &count);
	for (unsigned i=0;i<count;i++)
	{
#ifdef _GLOBAL_CODECS_AVAILABLE
		if (strstr(_GLOBAL_CODECS_AVAILABLE,PjToStr(&codec_info[i].codec_id)))
		{
#endif
			if (codec_info[i].priority
#ifdef _GLOBAL_CODECS_ENABLED
				&& (accountSettings.audioCodecs!="" || strstr(_GLOBAL_CODECS_ENABLED,PjToStr(&codec_info[i].codec_id)))
#endif
				)
			{
				listbox2->AddString(PjToStr(&codec_info[i].codec_id));
			} else
			{
				listbox->AddString(PjToStr(&codec_info[i].codec_id));
			}	
#ifdef _GLOBAL_CODECS_AVAILABLE
		}
#endif
	}

#ifdef _GLOBAL_VIDEO
	((CButton*)GetDlgItem(IDC_DISABLE_H264))->SetCheck(accountSettings.disableH264);
 	((CButton*)GetDlgItem(IDC_DISABLE_H263))->SetCheck(accountSettings.disableH263);
	if (accountSettings.bitrateH264=="") {
		const pj_str_t codec_id = {"H264", 4};
		pjmedia_vid_codec_param param;
		pjsua_vid_codec_get_param(&codec_id, &param);
		accountSettings.bitrateH264.Format("%d",param.enc_fmt.det.vid.max_bps/1000);
	}
	if (accountSettings.bitrateH263=="") {
		const pj_str_t codec_id = {"H263", 4};
		pjmedia_vid_codec_param param;
		pjsua_vid_codec_get_param(&codec_id, &param);
		accountSettings.bitrateH263.Format("%d",param.enc_fmt.det.vid.max_bps/1000);
	}
	GetDlgItem(IDC_BITRATE_264)->SetWindowText(accountSettings.bitrateH264);
	GetDlgItem(IDC_BITRATE_263)->SetWindowText(accountSettings.bitrateH263);

	combobox= (CComboBox*)GetDlgItem(IDC_VID_CAP_DEV);
	combobox->AddString(Translate("Default"));
	combobox->SetCurSel(0);
	pjmedia_vid_dev_info vid_dev_info[64];
	count = 64;
	pjsua_vid_enum_devs(vid_dev_info, &count);
	for (unsigned i=0;i<count;i++)
	{
		if (vid_dev_info[i].fmt_cnt && (vid_dev_info[i].dir==PJMEDIA_DIR_ENCODING || vid_dev_info[i].dir==PJMEDIA_DIR_ENCODING_DECODING))
		{
			combobox->AddString(vid_dev_info[i].name);
			if (!accountSettings.videoCaptureDevice.Compare(vid_dev_info[i].name))
			{
				combobox->SetCurSel(combobox->GetCount()-1);
			}
		}
	}

	combobox= (CComboBox*)GetDlgItem(IDC_VIDEO_CODEC);
	combobox->AddString(Translate("Default"));
	combobox->SetCurSel(0);
	count = 64;
	pjsua_vid_enum_codecs(codec_info, &count);
	for (unsigned i=0;i<count;i++)
	{
		combobox->AddString(PjToStr(&codec_info[i].codec_id));
		if (!accountSettings.videoCodec.Compare(PjToStr(&codec_info[i].codec_id)))
		{
			combobox->SetCurSel(combobox->GetCount()-1);
		}
	}
#endif

#ifndef _GLOBAL_CUSTOM
	((CButton*)GetDlgItem(IDC_DISABLE_LOCAL))->SetCheck(accountSettings.disableLocalAccount);
	
	int i;
	combobox= (CComboBox*)GetDlgItem(IDC_UPDATES_INTERVAL);
	combobox->AddString(Translate("Daily"));
	combobox->AddString(Translate("Weekly"));
	combobox->AddString(Translate("Monthly"));
	combobox->AddString(Translate("Quarterly"));
	combobox->AddString(Translate("Never"));
	if (accountSettings.updatesInterval=="daily")
	{
		i=0;
	} else if (accountSettings.updatesInterval=="monthly")
	{
		i=2;
	} else if (accountSettings.updatesInterval=="quarterly")
	{
		i=3;
	} else if (accountSettings.updatesInterval=="never")
	{
		i=4;
	} else
	{
		i=1;
	}
	combobox->SetCurSel(i);
#endif

	return TRUE;
}

void SettingsDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(SettingsDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &SettingsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &SettingsDlg::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MODIFY, &SettingsDlg::OnDeltaposSpinModify)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ORDER, &SettingsDlg::OnDeltaposSpinOrder)
#ifndef _GLOBAL_CUSTOM
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_RINGING_SOUND, &SettingsDlg::OnNMClickSyslinkRingingSound)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_AUTO_ANSWER, &SettingsDlg::OnNMClickSyslinkAutoAnswer)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_LOCAL_DTMF, &SettingsDlg::OnNMClickSyslinkLocalDTMF)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISABLE_LOCAL, &SettingsDlg::OnNMClickSyslinkDisableLocal)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SINGLE_MODE, &SettingsDlg::OnNMClickSyslinkSingleMode)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISABLE_VAD, &SettingsDlg::OnNMClickSyslinkDisableVad)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISABLE_H264, &SettingsDlg::OnNMClickSyslinkDisableH264)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISABLE_H263, &SettingsDlg::OnNMClickSyslinkDisableH263)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_AUDIO_CODECS, &SettingsDlg::OnNMClickSyslinkAudioCodecs)
#endif
#ifdef _GLOBAL_VIDEO
	ON_BN_CLICKED(IDC_PREVIEW, &SettingsDlg::OnBnClickedPreview)
#endif
#ifndef _GLOBAL_NO_RINGING_SOUND
	ON_BN_CLICKED(IDC_BROWSE, &SettingsDlg::OnBnClickedBrowse)
	ON_EN_CHANGE(IDC_RINGING_SOUND, &SettingsDlg::OnEnChangeRingingSound)
	ON_BN_CLICKED(IDC_DEFAULT, &SettingsDlg::OnBnClickedDefault)
#endif
END_MESSAGE_MAP()


void SettingsDlg::OnClose() 
{
	DestroyWindow();
}

void SettingsDlg::OnBnClickedCancel()
{
	OnClose();
}

void SettingsDlg::OnBnClickedOk()
{
#ifndef _GLOBAL_NO_AUTO
	accountSettings.autoAnswer=((CButton*)GetDlgItem(IDC_AUTO_ANSWER))->GetCheck();
#endif
	accountSettings.localDTMF=((CButton*)GetDlgItem(IDC_LOCAL_DTMF))->GetCheck();
#ifndef _GLOBAL_SINGLE_MODE
	accountSettings.singleMode=((CButton*)GetDlgItem(IDC_SINGLE_MODE))->GetCheck();
#endif
#ifndef _GLOBAL_NO_LOG
	accountSettings.enableLog=((CButton*)GetDlgItem(IDC_ENABLE_LOG))->GetCheck();
#endif
#ifndef _GLOBAL_NO_VAD
	accountSettings.disableVAD=((CButton*)GetDlgItem(IDC_DISABLE_VAD))->GetCheck();
#endif

	GetDlgItem(IDC_MICROPHONE)->GetWindowText(accountSettings.audioInputDevice);
	if (accountSettings.audioInputDevice==Translate("Default"))
	{
		accountSettings.audioInputDevice = "";
	}

	GetDlgItem(IDC_SPEAKERS)->GetWindowText(accountSettings.audioOutputDevice);
	if (accountSettings.audioOutputDevice==Translate("Default"))
	{
		accountSettings.audioOutputDevice = "";
	}

	accountSettings.audioCodecs = "";
	CListBox *listbox2;
	listbox2 = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS);
	for (unsigned i = 0; i < listbox2->GetCount(); i++)
	{
		CString str;
		listbox2->GetText(i, str);
		accountSettings.audioCodecs += str + " ";
	}
	accountSettings.audioCodecs.Trim();


#ifdef _GLOBAL_VIDEO
	accountSettings.disableH264=((CButton*)GetDlgItem(IDC_DISABLE_H264))->GetCheck();
	accountSettings.disableH263=((CButton*)GetDlgItem(IDC_DISABLE_H263))->GetCheck();
	GetDlgItem(IDC_BITRATE_264)->GetWindowText(accountSettings.bitrateH264);
	if (!atoi(accountSettings.bitrateH264)) {
		accountSettings.bitrateH264="";
	}
	GetDlgItem(IDC_BITRATE_263)->GetWindowText(accountSettings.bitrateH263);
	if (!atoi(accountSettings.bitrateH263)) {
		accountSettings.bitrateH263="";
	}
	GetDlgItem(IDC_VID_CAP_DEV)->GetWindowText(accountSettings.videoCaptureDevice);
	if (accountSettings.videoCaptureDevice==Translate("Default"))
	{
		accountSettings.videoCaptureDevice = "";
	}

	GetDlgItem(IDC_VIDEO_CODEC)->GetWindowText(accountSettings.videoCodec);
	if (accountSettings.videoCodec==Translate("Default"))
	{
		accountSettings.videoCodec = "";
	}
#endif

#ifndef _GLOBAL_NO_RINGING_SOUND
	GetDlgItem(IDC_RINGING_SOUND)->GetWindowText(accountSettings.ringingSound);
#endif

#ifndef _GLOBAL_CUSTOM
	accountSettings.disableLocalAccount=((CButton*)GetDlgItem(IDC_DISABLE_LOCAL))->GetCheck();
	
	CComboBox *combobox;
	combobox= (CComboBox*)GetDlgItem(IDC_UPDATES_INTERVAL);
	int i = combobox->GetCurSel();
	switch (i) {
		case 0:
			accountSettings.updatesInterval="daily";
			break;
		case 2:
			accountSettings.updatesInterval="monthly";
			break;
		case 3:
			accountSettings.updatesInterval="quarterly";
			break;
		case 4:
			accountSettings.updatesInterval="never";
			break;
		default:
			accountSettings.updatesInterval="";
	}
#endif

	accountSettings.Save();

	this->ShowWindow(SW_HIDE);

	microsipDlg->PJDestroy();
	microsipDlg->PJCreate();
	microsipDlg->PJAccountAdd();

	OnClose();
}

#ifndef _GLOBAL_NO_RINGING_SOUND
void SettingsDlg::OnBnClickedBrowse()
{
	CFileDialog dlgFile( TRUE, _T("wav"), 0, OFN_NOCHANGEDIR, _T("WAV Files (*.wav)|*.wav|") );
	if (dlgFile.DoModal()==IDOK) {
		CString cwd;
		char * ptr = cwd.GetBuffer(MAX_PATH);
		::GetCurrentDirectory(MAX_PATH, ptr);
		cwd.ReleaseBuffer();
		if ( cwd.MakeLower() + "\\" + dlgFile.GetFileName().MakeLower() == dlgFile.GetPathName().MakeLower() ) {
			GetDlgItem(IDC_RINGING_SOUND)->SetWindowText(dlgFile.GetFileName());
		} else {
			GetDlgItem(IDC_RINGING_SOUND)->SetWindowText(dlgFile.GetPathName());
		}
	}
}

void SettingsDlg::OnEnChangeRingingSound()
{
	CString str;
	GetDlgItem(IDC_RINGING_SOUND)->GetWindowText(str);
	GetDlgItem(IDC_DEFAULT)->EnableWindow(str.GetLength()>0);
}

void SettingsDlg::OnBnClickedDefault()
{
	GetDlgItem(IDC_RINGING_SOUND)->SetWindowText(NULL);
}
#endif

void SettingsDlg::OnDeltaposSpinModify(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CListBox *listbox;
	CListBox *listbox2;
	listbox = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS_ALL);
	listbox2 = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS);
	if (pNMUpDown->iDelta == -1) {
		//add
		int selected = listbox->GetCurSel();
		if (selected != LB_ERR) 
		{
			CString str;
			listbox->GetText(selected, str);
			listbox2->AddString(str);
			listbox->DeleteString(selected);
			listbox->SetCurSel( selected < listbox->GetCount() ? selected : selected-1 );
		}
	} else {
		//remove
		int selected = listbox2->GetCurSel();
		if (selected != LB_ERR) 
		{
			CString str;
			listbox2->GetText(selected, str);
			listbox->AddString(str);
			listbox2->DeleteString(selected);
			listbox2->SetCurSel( selected < listbox2->GetCount() ? selected : selected-1 );
		}
	}
	*pResult = 0;
}

void SettingsDlg::OnDeltaposSpinOrder(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CListBox *listbox2;
	listbox2 = (CListBox*)GetDlgItem(IDC_AUDIO_CODECS);
	int selected = listbox2->GetCurSel();
	if (selected != LB_ERR) 
	{
		CString str;
		listbox2->GetText(selected, str);
		if (pNMUpDown->iDelta == -1) {
			//up
			if (selected > 0)
			{
				listbox2->DeleteString(selected);
				listbox2->InsertString(selected-1,str);
				listbox2->SetCurSel(selected-1);
			}
		} else {
			//down
			if (selected < listbox2->GetCount()-1)
			{
				listbox2->DeleteString(selected);
				listbox2->InsertString(selected+1,str);
				listbox2->SetCurSel(selected+1);
			}
		}
	}
	*pResult = 0;
}

#ifndef _GLOBAL_CUSTOM

void SettingsDlg::OnNMClickSyslinkRingingSound(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("ringingSound");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkAutoAnswer(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("autoAnswer");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkLocalDTMF(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("localDTMF");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkDisableLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("disableLocal");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkSingleMode(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("singleMode");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkDisableVad(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("disableVAD");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkDisableH264(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("disableH264");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkDisableH263(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("disableH263");
	*pResult = 0;
}

void SettingsDlg::OnNMClickSyslinkAudioCodecs(NMHDR *pNMHDR, LRESULT *pResult)
{
	OpenHelp("audioCodecs");
	*pResult = 0;
}

#endif

#ifdef _GLOBAL_VIDEO
void SettingsDlg::OnBnClickedPreview()
{
	CComboBox *combobox;
	combobox = (CComboBox*)GetDlgItem(IDC_VID_CAP_DEV);
	CString name;
	combobox->GetWindowText(name);
	if (!microsipDlg->previewWin) {
		microsipDlg->previewWin = new Preview(microsipDlg);
	}
	microsipDlg->previewWin->Start(microsipDlg->VideoCaptureDeviceId(name));
}
#endif
