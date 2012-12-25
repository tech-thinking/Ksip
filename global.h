#pragma once

#include "const.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

enum {MSIP_MESSAGE_TYPE_LOCAL, MSIP_MESSAGE_TYPE_REMOTE, MSIP_MESSAGE_TYPE_SYSTEM};
enum {MSIP_TRANSPORT_AUTO, MSIP_TRANSPORT_TCP, MSIP_TRANSPORT_TLS};
enum {MSIP_SOUND_STOP, MSIP_SOUND_RING, MSIP_SOUND_RINGIN, MSIP_SOUND_RINGOUT, MSIP_SOUND_CUSTOM};

struct SIPURI {
	CString user;
	CString domain;
	CString name;
};

struct Contact {
	CString number;
	CString name;
	BOOL presence;
	time_t presenceTime;
};

struct MessagesContact {
	CString name;
	CString number;
	CString messages;
	CString message;
	CString prevMessage;
	pjsua_call_id callId;
};


struct my_call_data
{
   pj_pool_t          *pool;
   pjmedia_port       *tonegen;
   pjsua_conf_port_id  toneslot;
};

extern struct my_call_data *tone_gen;

void ParseSIPURI(CString in, SIPURI* out);
CString PjToStr(const pj_str_t* str, BOOL utf = FALSE);
char* StrToPj(CString str);
void OpenURL(CString url);

struct my_call_data *call_init_tonegen(pjsua_call_id call_id);
BOOL call_play_digit(pjsua_call_id call_id, const char *digits);
void call_deinit_tonegen(pjsua_call_id call_id);

#ifndef _GLOBAL_CUSTOM

void OpenHelp(CString code);

#endif
