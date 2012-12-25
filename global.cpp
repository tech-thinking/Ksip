#include "stdafx.h"
#include "global.h"
#include "settings.h"
#include "utf.h"

struct my_call_data *tone_gen = NULL;

void ParseSIPURI(CString in, SIPURI* out)
{
	//	tone_gen.toneslot = -1;
	//	tone_gen = NULL;

	// "WEwewew rewewe" <sip:qqweqwe@qwerer.com;qweqwe=rrr>
	out->domain = "";
	out->user = "";
	out->name = "";

	int start = in.Find( "sip:" );
	int end;
	if (start>=0)
	{
		if (start>0)
		{
			out->name = in.Left(start);
			out->name.Trim(" \" <");
			if (!out->name.CompareNoCase("unknown"))
			{
				out->name = "";
			}
		}
		start+=4;
		end = in.Find( "@", start );
		if (end>=0)
		{
			out->user=in.Mid(start,end-start);
			start=end+1;
		}
		end = in.Find( ";", start );
		if (end<0)
		{
			end = in.Find( ">", start );
		}
		if (end>=0)
		{
			out->domain=in.Mid(start,end-start);
		} else {
			out->domain=in.Mid(start);
		}
	}
}

CString PjToStr(const pj_str_t* str, BOOL utf)
{
	CString rab;
	rab.Format("%.*s", str->slen, str->ptr);
	if (utf)
	{
		return Utf8DecodeCP(rab.GetBuffer(), CP_ACP, NULL);
	} else 
	{
		return rab;
	}
}

char* StrToPj(CString str)
{
	return Utf8EncodeCP(str.GetBuffer(), CP_ACP);
}

void OpenURL(CString url)
{
	CHAR szTemp[256];
	sprintf(szTemp, "url.dll,FileProtocolHandler %s", url);
	ShellExecute(NULL, NULL, "rundll32.exe", szTemp, NULL, SW_SHOWNORMAL);
}

#ifndef _GLOBAL_CUSTOM

void OpenHelp(CString code)
{
	OpenURL("http://microsip.org.ua/help#"+code);
}

#endif

struct my_call_data *call_init_tonegen(pjsua_call_id call_id)
{
	pj_pool_t *pool;
	struct my_call_data *cd;
	pjsua_call_info ci;

	if (call_id !=-1 ) {
		pjsua_call_get_info(call_id, &ci);

		if (ci.media_status != PJSUA_CALL_MEDIA_ACTIVE)
			return NULL;
	}

	pool = pjsua_pool_create("mycall", 512, 512);
	cd = PJ_POOL_ZALLOC_T(pool, struct my_call_data);
	cd->pool = pool;

	pjmedia_tonegen_create(cd->pool, 8000, 1, 160, 16, 0, &cd->tonegen);
	pjsua_conf_add_port(cd->pool, cd->tonegen, &cd->toneslot);

	if (call_id !=-1 ) {
		pjsua_conf_connect(cd->toneslot, ci.conf_slot);
	}
	if (accountSettings.localDTMF) {
		pjsua_conf_connect(cd->toneslot, 0);
	}

	if (call_id !=-1 ) {
		pjsua_call_set_user_data(call_id, (void*) cd);
	}
	return cd;
}

BOOL call_play_digit(pjsua_call_id call_id, const char *digits)
{
	pjmedia_tone_digit d[16];
	unsigned i, count = strlen(digits);
	struct my_call_data *cd;

	if (call_id !=-1 ) {
		cd = (struct my_call_data*) pjsua_call_get_user_data(call_id);
	} else {
		cd = tone_gen;
	}
	if (!cd)
		cd = call_init_tonegen(call_id);
	if (!cd) 
		return FALSE;
	if (call_id == -1 ) {
		tone_gen = cd;
	}

	if (count > PJ_ARRAY_SIZE(d))
		count = PJ_ARRAY_SIZE(d);

	pj_bzero(d, sizeof(d));
	for (i=0; i<count; ++i) {
		d[i].digit = digits[i];
		if (call_id !=-1 ) {
			d[i].on_msec = 260;
			d[i].off_msec = 200;
		} else {
			d[i].on_msec = 120;
			d[i].off_msec = 50;
		}
		d[i].volume = 0;
	}

	pjmedia_tonegen_play_digits(cd->tonegen, count, d, 0);
	return TRUE;
}

void call_deinit_tonegen(pjsua_call_id call_id)
{
	struct my_call_data *cd;

	if (call_id !=-1 ) {
		cd = (struct my_call_data*) pjsua_call_get_user_data(call_id);
	} else {
		cd = tone_gen;
	}
	if (!cd)
		return;

	pjsua_conf_remove_port(cd->toneslot);
	pjmedia_port_destroy(cd->tonegen);
	pj_pool_release(cd->pool);

	if (call_id !=-1 ) {
		pjsua_call_set_user_data(call_id, NULL);
	} else {
		tone_gen = NULL;
	}
}
