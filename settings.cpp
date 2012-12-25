#include "stdafx.h"
#include "settings.h"
#include "global.h"

AccountSettings accountSettings;
BOOL firstRun;

AccountSettings::AccountSettings()
{
	logFile.Format("%s.log", _GLOBAL_NAME);
	iniFile.Format("%s.ini", _GLOBAL_NAME);

	CFileStatus rStatus;
	CString str;
	CString appData;
	char * ptr;
	ptr = appData.GetBuffer(MAX_PATH);
	::GetCurrentDirectory(MAX_PATH, ptr);
	appData.ReleaseBuffer();
	appData += "\\";

	firstRun = TRUE;
	if (CFile::GetStatus(appData + iniFile, rStatus)) {
		firstRun = FALSE;
	}

	HANDLE h = CreateFile( appData + iniFile, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ |
		 FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(h);
	} else {
		ptr = appData.GetBuffer(MAX_PATH);
		SHGetSpecialFolderPath(
			0,
			ptr, 
			CSIDL_LOCAL_APPDATA, 
			FALSE ); 
		appData.ReleaseBuffer();
		appData.AppendFormat("\\%s\\",_GLOBAL_NAME_NICE);
		CreateDirectory(appData, NULL);
		if (CFile::GetStatus(appData + iniFile, rStatus)) {
			firstRun = FALSE;
		}
	}
	logFile = appData + logFile;
	iniFile = appData + iniFile;

#ifndef _GLOBAL_ACCOUNT_MINI
	ptr = server.GetBuffer(255);
	GetPrivateProfileString("Settings","Server", NULL, ptr, 256, iniFile);
	server.ReleaseBuffer();
	ptr = proxy.GetBuffer(255);
	GetPrivateProfileString("Settings","Proxy", NULL, ptr, 256, iniFile);
	proxy.ReleaseBuffer();
	ptr = domain.GetBuffer(255);
	GetPrivateProfileString("Settings","Domain", NULL, ptr, 256, iniFile);
	domain.ReleaseBuffer();
	ptr = srtp.GetBuffer(255);
	GetPrivateProfileString("Settings","SRTP", NULL, ptr, 256, iniFile);
	srtp.ReleaseBuffer();
	ptr = transport.GetBuffer(255);
	GetPrivateProfileString("Settings","Transport", NULL, ptr, 256, iniFile);
	transport.ReleaseBuffer();
	ptr = publicAddr.GetBuffer(255);
	GetPrivateProfileString("Settings","PublicAddr", NULL, ptr, 256, iniFile);
	publicAddr.ReleaseBuffer();
	ptr = listenPort.GetBuffer(255);
	GetPrivateProfileString("Settings","ListenPort", NULL, ptr, 256, iniFile);
	listenPort.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","Publish", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	publish=str=="1"?1:0;
	ptr = stun.GetBuffer(255);
	GetPrivateProfileString("Settings","STUN", NULL, ptr, 256, iniFile);
	stun.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","ICE", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	ice=str=="1"?1:0;

	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","DisableLocalAccount", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	disableLocalAccount=str=="1"?1:0;
	
	ptr = updatesInterval.GetBuffer(255);
	GetPrivateProfileString("Settings","UpdatesInterval", NULL, ptr, 256, iniFile);
	updatesInterval.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","CheckUpdatesTime", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	checkUpdatesTime = atoi(str);
#else
	server = _GLOBAL_ACCOUNT_SIP_SERVER;
	proxy = _GLOBAL_ACCOUNT_SIP_PROXY;
	domain= _GLOBAL_ACCOUNT_DOMAIN;
	srtp = _GLOBAL_ACCOUNT_SRTP;
	transport = _GLOBAL_ACCOUNT_TRANSPORT;
	publish=_GLOBAL_ACCOUNT_PUBLISH;
	stun = _GLOBAL_ACCOUNT_STUN;
	ice=_GLOBAL_ACCOUNT_ICE;
	disableLocalAccount=1;
#endif

	ptr = username.GetBuffer(255);
	GetPrivateProfileString("Settings","Username", NULL, ptr, 256, iniFile);
	username.ReleaseBuffer();
	ptr = authID.GetBuffer(255);
	GetPrivateProfileString("Settings","AuthID", NULL, ptr, 256, iniFile);
	authID.ReleaseBuffer();
	ptr = password.GetBuffer(255);
	GetPrivateProfileString("Settings","Password", NULL, ptr, 256, iniFile);
	password.ReleaseBuffer();
	ptr = displayName.GetBuffer(255);
	GetPrivateProfileString("Settings","DisplayName", NULL, ptr, 256, iniFile);
	displayName.ReleaseBuffer();

	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","AutoAnswer", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	autoAnswer=str=="1"?1:0;
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","LocalDTMF", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	localDTMF=str=="0"?0:1;
	
	ptr = ringingSound.GetBuffer(255);
	GetPrivateProfileString("Settings","RingingSound", NULL, ptr, 256, iniFile);
	ringingSound.ReleaseBuffer();
	ptr = audioInputDevice.GetBuffer(255);
	GetPrivateProfileString("Settings","AudioInputDevice", NULL, ptr, 256, iniFile);
	audioInputDevice.ReleaseBuffer();
	ptr = audioOutputDevice.GetBuffer(255);
	GetPrivateProfileString("Settings","AudioOutputDevice", NULL, ptr, 256, iniFile);
	audioOutputDevice.ReleaseBuffer();
	ptr = audioCodecs.GetBuffer(255);
	GetPrivateProfileString("Settings","AudioCodecs", NULL, ptr, 256, iniFile);
	audioCodecs.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","DisableVAD", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	disableVAD = str == "1" ? 1 : 0;

	ptr = videoCaptureDevice.GetBuffer(255);
	GetPrivateProfileString("Settings","VideoCaptureDevice", NULL, ptr, 256, iniFile);
	videoCaptureDevice.ReleaseBuffer();
	ptr = videoCodec.GetBuffer(255);
	GetPrivateProfileString("Settings","VideoCodec", NULL, ptr, 256, iniFile);
	videoCodec.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","DisableH264", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	disableH264 = str == "1" ? 1 : 0;
	ptr = bitrateH264.GetBuffer(255);
	GetPrivateProfileString("Settings","BitrateH264", NULL, ptr, 256, iniFile);
	bitrateH264.ReleaseBuffer();
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","DisableH263", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	disableH263 = str == "1" ? 1 : 0;
	ptr = bitrateH263.GetBuffer(255);
	GetPrivateProfileString("Settings","BitrateH263", NULL, ptr, 256, iniFile);
	bitrateH263.ReleaseBuffer();

	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","ActiveTab", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	activeTab = atoi(str);

#ifndef _GLOBAL_NO_LOG
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","EnableLog", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	enableLog=str=="1"?1:0;
#else
	enableLog=0;
#endif

#ifndef _GLOBAL_SINGLE_MODE
	ptr = str.GetBuffer(255);
	GetPrivateProfileString("Settings","SingleMode", NULL, ptr, 256, iniFile);
	str.ReleaseBuffer();
	singleMode=str=="1"?1:0;
#else
	singleMode = 1;
#endif

	hidden = 0;

}

void AccountSettings::Save()
{
	CString str;

#ifndef _GLOBAL_ACCOUNT_MINI
	WritePrivateProfileString("Settings","Server",server,iniFile);
	WritePrivateProfileString("Settings","Proxy",proxy,iniFile);
	WritePrivateProfileString("Settings","Domain",domain,iniFile);
	WritePrivateProfileString("Settings","SRTP",srtp,iniFile);
	WritePrivateProfileString("Settings","Transport",transport,iniFile);
	WritePrivateProfileString("Settings","PublicAddr",publicAddr,iniFile);
	WritePrivateProfileString("Settings","ListenPort",listenPort,iniFile);
	WritePrivateProfileString("Settings","Publish",publish?"1":"0",iniFile);
	WritePrivateProfileString("Settings","STUN",stun,iniFile);
	WritePrivateProfileString("Settings","ICE",ice?"1":"0",iniFile);

	WritePrivateProfileString("Settings","DisableLocalAccount",disableLocalAccount?"1":"0",iniFile);

	WritePrivateProfileString("Settings","UpdatesInterval",updatesInterval,iniFile);
	str.Format("%d",checkUpdatesTime);
	WritePrivateProfileString("Settings","CheckUpdatesTime",str,iniFile);
#endif

#ifndef _GLOBAL_NO_LOG
	WritePrivateProfileString("Settings","EnableLog",enableLog?"1":"0",iniFile);
#endif

	WritePrivateProfileString("Settings","Username",username,iniFile);
	WritePrivateProfileString("Settings","AuthID",authID,iniFile);
	WritePrivateProfileString("Settings","Password",password,iniFile);
	WritePrivateProfileString("Settings","DisplayName",displayName,iniFile);
	
	WritePrivateProfileString("Settings","AutoAnswer",autoAnswer?"1":"0",iniFile);
#ifndef _GLOBAL_SINGLE_MODE
	WritePrivateProfileString("Settings","SingleMode",singleMode?"1":"0",iniFile);
#endif
	WritePrivateProfileString("Settings","LocalDTMF",localDTMF?"1":"0",iniFile);
	WritePrivateProfileString("Settings","RingingSound",ringingSound,iniFile);
	WritePrivateProfileString("Settings","AudioInputDevice",audioInputDevice,iniFile);
	WritePrivateProfileString("Settings","AudioOutputDevice",audioOutputDevice,iniFile);
	WritePrivateProfileString("Settings","AudioCodecs",audioCodecs,iniFile);
	WritePrivateProfileString("Settings","DisableVAD",disableVAD?"1":"0",iniFile);
	WritePrivateProfileString("Settings","VideoCaptureDevice",videoCaptureDevice,iniFile);
	WritePrivateProfileString("Settings","VideoCodec",videoCodec,iniFile);
	WritePrivateProfileString("Settings","DisableH264",disableH264?"1":"0",iniFile);
	WritePrivateProfileString("Settings","BitrateH264",bitrateH264,iniFile);
	WritePrivateProfileString("Settings","DisableH263",disableH263?"1":"0",iniFile);
	WritePrivateProfileString("Settings","BitrateH263",bitrateH263,iniFile);
		
	str.Format("%d",activeTab);
	WritePrivateProfileString("Settings","ActiveTab",str,iniFile);

}
