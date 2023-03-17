#pragma once

#include <Windows.h>
#include <winsvc.h>
#include <WinIoCtl.h>
#include "../Common/Define.h"
#include "ErrorMsg.h"
#include "InstallDrv.h"
#include "CDownloadPDB.h"
#include "../Common/CDumpPdb.h"


extern HANDLE g_hDevice;
extern CDownloadPDB g_pdbNtkrnl;
extern CDownloadPDB g_pdbWin32k;
extern CDumpPdb g_symNtkrnl;
extern CDumpPdb g_symWin32k;
extern PVOID g_pOridinalNtoskrnl;			// ntoskrnl.exe文件缓冲区
extern PVOID g_pOridinalWin32k;			// g_pOridinalWin32k.sys文件缓冲区
extern PVOID g_pNtoskrnlBase;
extern PVOID g_pWin32kBase;
extern PVOID g_pNtoskrnlImgBase;
extern PVOID g_pWin32kImgBase;
extern CList<SECTION, SECTION&> g_NtoskrnlCheckList;
extern CList<SECTION, SECTION&> g_Win32kCheckList;

#define PATH_NTOSKRNL       L"C:\\Windows\\System32\\ntoskrnl.exe"
#define PATH_WIN32K         L"C:\\Windows\\System32\\win32k.sys"
#define LINKPATH_NTOSKRNL   L"\\??\\C:\\Windows\\System32\\ntoskrnl.exe";
#define LINKPATH_WIN32K     L"\\??\\C:\\Windows\\System32\\win32k.sys"

#define PSP_MAX_CREATE_PROCESS_NOTIFY 64
#define PSP_MAX_CREATE_THREAD_NOTIFY 64