

#include <windows.h>
#include <winsock.h>

#include "neflib.h"
#include "xmpdsp.h"
#include "last.fm.h"

char *buff;
char *prev;

static HANDLE hwnd_xmp;
static HANDLE hwnd_bot;
static HHOOK hook;
static HANDLE poster;
static DWORD poster_id;

static void WINAPI DSP_About(HWND win);
static void *WINAPI DSP_New(void);
static void WINAPI DSP_Free(void *inst);
static const char *WINAPI DSP_GetDescription(void *inst);
static void WINAPI DSP_Config(void *inst, HWND win);
static DWORD WINAPI DSP_GetConfig(void *inst, void *config);
static BOOL WINAPI DSP_SetConfig(void *inst, void *config, DWORD size);
static void WINAPI DSP_NewTrack(void *inst, char *file);
static void WINAPI DSP_SetFormat(void *inst, const XMPFORMAT *form) {};
static void WINAPI DSP_Reset(void *inst) {};
static DWORD WINAPI DSP_Process(void *inst, float *srce, DWORD count) {return 0;};

static void lookup_bot();
static DWORD __stdcall PostTrackInfo(LPVOID param);

/* --------------------------------------------------------------
                            Structures
   -------------------------------------------------------------- */

static XMPDSP dsp = {
    0,
    "XMPlay2IRC",
    DSP_About,
    DSP_New,
    DSP_Free,
    DSP_GetDescription,
    DSP_Config,
    DSP_GetConfig,
    DSP_SetConfig,
    DSP_NewTrack,
    DSP_SetFormat,
    DSP_Reset,
    DSP_Process
};

/* --------------------------------------------------------------
                            Constants
   -------------------------------------------------------------- */

#define BUFF_SIZE 1024

/* -----------------------------------------------------------------------
                           Plugin interface
   ----------------------------------------------------------------------- */

static void WINAPI DSP_About(HWND win)
{
	MessageBox(win,
		"XMPlay to IRC /me playing \n© 2008, itanko",
		"XMPlay IRC (ver 0.1)",
		MB_ICONINFORMATION);
}

static const char *WINAPI DSP_GetDescription(void *inst)
{
    return dsp.name;
}

static void *WINAPI DSP_New()
{
	buff = malloc(BUFF_SIZE);
	prev = malloc(BUFF_SIZE); *prev=0;
	hwnd_xmp = FindWindow("XMPLAY-MAIN",0);
	lookup_bot();
	poster = CreateThread(0,0,&PostTrackInfo,0,CREATE_SUSPENDED,&poster_id);

	ResumeThread(poster);
}

static void WINAPI DSP_Free(void *inst)
{
	//
}

static void WINAPI DSP_Config(void *inst, HWND win)
{
    dsp.About(win);
}

static DWORD WINAPI DSP_GetConfig(void *inst, void *config)
{
    return 0; // return size of config info
}

static BOOL WINAPI DSP_SetConfig(void *inst, void *config, DWORD size)
{
    return TRUE;
}

static void WINAPI DSP_NewTrack(void *inst, char *file)
{
	ResumeThread(poster);
}

XMPDSP *APIENTRY __declspec(dllexport)
XMPDSP_GetInterface2(DWORD face, InterfaceProc faceproc)
{
	return &dsp;
}

/* -----------------------------------------------------------------------
                           Internal routines
   ----------------------------------------------------------------------- */

static void lookup_bot() {
	hwnd_bot = FindWindow("ITANKOS-BOT",0);
	if(hwnd_bot) {
		SendMessageA(hwnd_bot, WM_USER+ITB_NOTIFY_XMPLAY, (unsigned int)hwnd_xmp, 0);
	}
}

static DWORD __stdcall PostTrackInfo(LPVOID param)
{
	for(;;) {
		Sleep(5000);
		GetWindowTextA(hwnd_xmp,buff,BUFF_SIZE);
		if(!strcmp(buff, prev)) {
			continue;
		}
		strcpy(prev, buff);
		if(!hwnd_bot) lookup_bot();
		if(hwnd_bot) {
			SetWindowTextA(hwnd_bot,buff);
			PostMessage(hwnd_bot,WM_USER+ITB_SHOW_TACKINFO,(unsigned int)buff,BUFF_SIZE);
		}
	}
}

BOOL __stdcall dll_main(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hDLLInst);
    return 1;
}

