#pragma once

struct AccountSettings {
	CString server;
	CString proxy;
	CString srtp;
	CString transport;
	CString publicAddr;
	CString listenPort;
	int publish;
	CString stun;
	int ice;
	
    CString username;
	CString domain;
	CString authID;
	CString password;
	CString displayName;

	int autoAnswer;
	int singleMode;
	int disableLocalAccount;
	int enableLog;
	CString ringingSound;
	CString audioInputDevice;
	CString audioOutputDevice;
	CString audioCodecs;
	int disableVAD;
	CString videoCaptureDevice;
	CString videoCodec;
	int disableH264;
	CString bitrateH264;	
	int disableH263;
	CString bitrateH263;
	int localDTMF;

	CString updatesInterval;

	int activeTab;
	
	CString iniFile;
	CString logFile;
	int checkUpdatesTime;

	int hidden;

	AccountSettings();
	void Save();
};
extern AccountSettings accountSettings;
extern BOOL firstRun;