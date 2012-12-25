#pragma once

#include "const.h"

#ifdef _GLOBAL_VIDEO

#include "resource.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

class Preview: public CWnd
{
public:
	Preview(CWnd* pParent = NULL);
	~Preview();
	void Start(int id);
private:
	pjmedia_vid_dev_index widx;
protected:
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
};

#endif