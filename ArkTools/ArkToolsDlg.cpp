
// ArkToolsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ArkTools.h"
#include "ArkToolsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE g_hDevice = INVALID_HANDLE_VALUE;
CDownloadPDB g_pdbNtkrnl;
CDownloadPDB g_pdbWin32k;
CDumpPdb g_symNtkrnl;
CDumpPdb g_symWin32k;
PVOID g_pOridinalNtoskrnl = nullptr;			// ntoskrnl.exe文件缓冲区
PVOID g_pOridinalWin32k = nullptr;			// g_pOridinalWin32k.sys文件缓冲区
PVOID g_pNtoskrnlBase = 0;
PVOID g_pWin32kBase = 0;
PVOID g_pNtoskrnlImgBase = 0;
PVOID g_pWin32kImgBase = 0;
CList<SECTION, SECTION&> g_NtoskrnlCheckList;
CList<SECTION, SECTION&> g_Win32kCheckList;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CArkToolsDlg 对话框



CArkToolsDlg::CArkToolsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARKTOOLS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CArkToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, TAB_MAIN, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CArkToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE, TAB_MAIN, &CArkToolsDlg::OnSelchangeTabMain)
END_MESSAGE_MAP()


// CArkToolsDlg 消息处理程序

BOOL CArkToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CArkToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CArkToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CArkToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CArkToolsDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UnLoadDriver();
	if (g_pOridinalNtoskrnl != nullptr)
	{
		VirtualFree(g_pOridinalNtoskrnl, 0, MEM_RELEASE);
	}
	if (g_pOridinalWin32k != nullptr)
	{
		VirtualFree(g_pOridinalWin32k, 0, MEM_RELEASE);
	}
	CDialogEx::OnClose();
}


void CArkToolsDlg::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_TabDlgProcess.ShowWindow(SW_HIDE);
	m_TabDlgDriver.ShowWindow(SW_HIDE);
	m_TabDlgFile.ShowWindow(SW_HIDE);
	m_TabDlgReg.ShowWindow(SW_HIDE);
	m_TabDlgGDT.ShowWindow(SW_HIDE);
	m_TabDlgIDT.ShowWindow(SW_HIDE);
	m_TabDlgSSDT.ShowWindow(SW_HIDE);
	m_TabDlgUnLockFile.ShowWindow(SW_HIDE);
	m_TabDlgSysCallback.ShowWindow(SW_HIDE);

	switch (m_TabCtrl.GetCurSel())
	{
	case TT_PROCESS:
		m_TabDlgProcess.ShowWindow(SW_SHOW);
		m_TabDlgProcess.InsertListItem();
		break;
	case TT_DRIVER:
		m_TabDlgDriver.ShowWindow(SW_SHOW);
		m_TabDlgDriver.InsertListItem();
		break;
	case TT_FILE:
		m_TabDlgFile.ShowWindow(SW_SHOW);
		m_TabDlgFile.m_lcFile.DeleteAllItems();
		break;
	case TT_REGEDIT:
		m_TabDlgReg.ShowWindow(SW_SHOW);
		break;
	//case TT_SERVICE:
	//	m_TabDlgService.ShowWindow(SW_SHOW);
	//	m_TabDlgService.InsertService();
	//	break;
	//case TT_MSCONFIG:
	//	m_TabDlgMsconfig.ShowWindow(SW_SHOW);
	//	m_TabDlgMsconfig.InsertMsconfig();
	//	break;
	case TT_GDT:
		m_TabDlgGDT.ShowWindow(SW_SHOW);
		m_TabDlgGDT.InsertListItem();
		break;
	case TT_IDT:
		m_TabDlgIDT.ShowWindow(SW_SHOW);
		m_TabDlgIDT.InsertListItem();
		break;
	case TT_SSDT:
		m_TabDlgSSDT.ShowWindow(SW_SHOW);
		m_TabDlgSSDT.InsertListItem();
		break;
	//case TT_HOOK:
	//	m_TabDlgHook.ShowWindow(SW_SHOW);
	//	m_TabDlgHook.InsertHook();
		break;
	case TT_FILEUNLOCK:
		m_TabDlgUnLockFile.ShowWindow(SW_SHOW);
		break;
	case TT_SYSCALLBACK:
		m_TabDlgSysCallback.ShowWindow(SW_SHOW);
		m_TabDlgSysCallback.InsertListItem();
		break;
	default:
		break;
	}

	*pResult = 0;
}
/*================================================================*/

void CArkToolsDlg::Init()
{
	if (!InitPDB())
	{
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
		return;
	}
	if (!LoadDriver())
	{
		AfxGetMainWnd()->SendMessage(WM_CLOSE);	
		return;
	}
	if (!InitKrnlFile())
	{
		AfxMessageBox(L"Hook检测初始化失败！");
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
		return;
	}
	InitTabCtrl();
	return;
}

BOOL CArkToolsDlg::InitPDB()
{
	g_pdbNtkrnl.Init(PATH_NTOSKRNL);
	g_pdbWin32k.Init(PATH_WIN32K);
	if (!g_pdbNtkrnl.IsComplete() || !g_pdbWin32k.IsComplete())
	{
		AfxMessageBox(L"内核模块pdb加载失败！");
		return FALSE;
	}

	g_symNtkrnl.Init(g_pdbNtkrnl.GetPdbPath());
	g_symWin32k.Init(g_pdbWin32k.GetPdbPath());
	if (!g_symNtkrnl.IsInit() || !g_symWin32k.IsInit())
	{
		AfxMessageBox(L"内核模块符号加载失败！");
		return FALSE;
	}
	return TRUE;
}

void CArkToolsDlg::InitTabCtrl()
{
	CRect tabRect;
	m_TabCtrl.GetClientRect(&tabRect);
	tabRect.DeflateRect(2, 30, 5, 5);

	//进程标签
	m_TabCtrl.InsertItem(TT_PROCESS, L"进程");
	m_TabDlgProcess.Create(TAB_DLG_PROCESS, &m_TabCtrl);
	m_TabDlgProcess.MoveWindow(&tabRect);
	m_TabDlgProcess.ShowWindow(SW_SHOW);
	m_TabDlgProcess.InsertListItem();

	//驱动标签
	m_TabCtrl.InsertItem(TT_DRIVER, L"驱动");
	m_TabDlgDriver.Create(TAB_DLG_DRIVER, &m_TabCtrl);
	m_TabDlgDriver.MoveWindow(&tabRect);
	m_TabDlgDriver.ShowWindow(SW_HIDE);

	//文件标签
	m_TabCtrl.InsertItem(TT_FILE, L"文件");
	m_TabDlgFile.Create(TAB_DLG_FILE, &m_TabCtrl);
	m_TabDlgFile.MoveWindow(&tabRect);
	m_TabDlgFile.ShowWindow(SW_HIDE);

	////注册表标签
	m_TabCtrl.InsertItem(TT_REGEDIT, L"注册表");
	m_TabDlgReg.Create(TAB_DLG_REG, &m_TabCtrl);
	m_TabDlgReg.MoveWindow(&tabRect);
	m_TabDlgReg.ShowWindow(SW_HIDE);

	////GDT标签
	m_TabCtrl.InsertItem(TT_GDT, L"GDT");
	m_TabDlgGDT.Create(TAB_DLG_GDT, &m_TabCtrl);
	m_TabDlgGDT.MoveWindow(&tabRect);
	m_TabDlgGDT.ShowWindow(SW_HIDE);

	////IDT标签
	m_TabCtrl.InsertItem(TT_IDT, L"系统中断表");
	m_TabDlgIDT.Create(TAB_DLG_IDT, &m_TabCtrl);
	m_TabDlgIDT.MoveWindow(&tabRect);
	m_TabDlgIDT.ShowWindow(SW_HIDE);

	//SSDT标签
	m_TabCtrl.InsertItem(TT_SSDT, L"SSDT");
	m_TabDlgSSDT.Create(TAB_DLG_SSDT, &m_TabCtrl);
	m_TabDlgSSDT.MoveWindow(&tabRect);
	m_TabDlgSSDT.ShowWindow(SW_HIDE);

	//文件粉碎/解锁
	m_TabCtrl.InsertItem(TT_FILEUNLOCK, L"文件粉碎/解锁");
	m_TabDlgUnLockFile.Create(TAB_DLG_UNLOCK, &m_TabCtrl);
	m_TabDlgUnLockFile.MoveWindow(&tabRect);
	m_TabDlgUnLockFile.ShowWindow(SW_HIDE);

	//系统回调
	m_TabCtrl.InsertItem(TT_SYSCALLBACK, L"系统回调");
	m_TabDlgSysCallback.Create(TAB_DLG_SYSCALLBACK, &m_TabCtrl);
	m_TabDlgSysCallback.MoveWindow(&tabRect);
	m_TabDlgSysCallback.ShowWindow(SW_HIDE);
}

BOOL CArkToolsDlg::LoadDriver()
{
	BOOL bRet = FALSE;


	//从资源获取驱动文件
	HRSRC hSrc = FindResource(NULL, MAKEINTRESOURCE(DRV_ARK), L"Driver");
	if (hSrc == NULL)
	{
		return FALSE;
	}
	DWORD dwSize = SizeofResource(NULL, hSrc);
	HGLOBAL hGlobal = LoadResource(NULL, hSrc);
	if (hGlobal == NULL)
	{
		return FALSE;
	}
	LPVOID pBuf = LockResource(hGlobal);
	if (pBuf == NULL)
	{
		return FALSE;
	}
	HANDLE hFile = CreateFile(L"ArkDriver.sys", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD dwNumberOfBytesWrite = 0;
	if (!WriteFile(hFile, pBuf, dwSize, &dwNumberOfBytesWrite, NULL))
	{
		return FALSE;
	}
	if (dwNumberOfBytesWrite != dwSize)
	{
		return FALSE;
	}
	CloseHandle(hFile);

	//创建服务、启动驱动、获取设备句柄
	WCHAR szServiceName[] = L"ArkDriver";
	WCHAR szBuf[MAXBYTE] = { 0 };
	::GetModuleFileName(NULL, szBuf, MAXBYTE * sizeof(WCHAR));
	CString strPath = szBuf;
	strPath = strPath.Left(strPath.ReverseFind(L'\\'));
	strPath += L"\\ArkDriver.sys";

	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (m_hSCManager == NULL)
	{
		return FALSE;
	}

	scmUnloadDeviceDriver(szServiceName);	//总之先关一次服务，以免出现重复加载的问题
	if (!scmInstallDriver(m_hSCManager, szServiceName, strPath.GetBuffer()))	//CreateService
	{
		AfxMessageBox(L"创建服务失败！");
		return FALSE;
	}
	if (!scmStartDriver(m_hSCManager, szServiceName))		//startService
	{
		AfxMessageBox(L"驱动启动失败！");
		return FALSE;
	}
	if (!scmOpenDevice(szServiceName, &g_hDevice))			//CreateFile
	{
		AfxMessageBox(L"设备创建失败！");
		return FALSE;
	}
	DeleteFile(L"ArkDriver.sys");		//清理文件
	return TRUE;
}

BOOL CArkToolsDlg::UnLoadDriver()
{
	return scmUnloadDeviceDriver(L"ArkDriver");
}

BOOL CArkToolsDlg::InitKrnlFile()
{
	BOOL bRet = FALSE;
	wchar_t szKernelPath[] = LINKPATH_NTOSKRNL;
	wchar_t szWin32kPath[] = LINKPATH_WIN32K;
	CString strKernel = L"ntoskrnl.exe";
	CString strWin32k = L"win32k.sys";

	DRIVER_INFO* pDivInfo = nullptr;
	DWORD dwRetByte = 0;
	DWORD dwDivInfoSize = 0;
	CString fmt;

	//获取缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER, NULL, 0, NULL, 0, &dwRetByte, NULL))
	{
		OutputDebugString(L"[testR3] InitKrnlFile(): 获取驱动模块信息所需缓冲区大小失败!\r\n");
		return FALSE;
	}

	//获取信息
	dwDivInfoSize = dwRetByte;
	pDivInfo = (DRIVER_INFO*)new char[dwDivInfoSize];
	RtlZeroMemory(pDivInfo, dwDivInfoSize);
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER,
		NULL, 0,
		pDivInfo, dwDivInfoSize,
		&dwRetByte, NULL))
	{
		OutputDebugString(L"[testR3] InitKrnlFile(): 获取驱动模块信息失败!\r\n");
		goto InitKrnlFile_Ret;
	}

	DWORD dw = 0;
	for (DWORD i = 0; i < dwDivInfoSize / sizeof(DRIVER_INFO); i++)
	{
		if (strKernel == pDivInfo[i].m_szName)
		{
			g_pNtoskrnlBase = (PVOID)pDivInfo[i].m_nBase;
			dw++;
		}
		if (strWin32k == pDivInfo[i].m_szName)
		{
			g_pWin32kBase = (PVOID)pDivInfo[i].m_nBase;
			dw++;
		}
		if (dw == 2)
		{
			break;
		}
	}

	if (g_pNtoskrnlBase == 0 || g_pWin32kBase == 0)
	{
		fmt.Format(L"[testR3] InitKrnlFile(): 获取模块基址失败, pKernelBase=0x%p, pWin32kBase=0x%p\r\n", g_pNtoskrnlBase, g_pWin32kBase);
		OutputDebugString(fmt);
		goto InitKrnlFile_Ret;
	}

	g_pOridinalNtoskrnl = LoadPE(szKernelPath, sizeof(szKernelPath), (DWORD)g_pNtoskrnlBase, &g_NtoskrnlCheckList, &g_pNtoskrnlImgBase);
	g_pOridinalWin32k = LoadPE(szWin32kPath, sizeof(szWin32kPath), (DWORD)g_pWin32kBase, &g_Win32kCheckList, &g_pWin32kImgBase);

	if (g_pOridinalNtoskrnl == nullptr || g_pOridinalWin32k == nullptr)
	{
		fmt.Format(L"[testR3] InitKrnlFile(): 重定位内核模块失败, g_pNtoskrnl=0x%p, g_pWin32k=0x%p\r\n", g_pOridinalNtoskrnl, g_pOridinalWin32k);
		OutputDebugString(fmt);
		goto InitKrnlFile_Ret;
	}

	bRet = TRUE;
InitKrnlFile_Ret:
	if (pDivInfo != nullptr)
	{
		delete[]pDivInfo;
	}
	return bRet;
}

PVOID CArkToolsDlg::LoadPE(wchar_t* szPath, DWORD dwPathSize, DWORD dwRelocationBase, CList<SECTION, SECTION&>* pCheckList, PVOID* pImgBase)
{
	CString fmt;
	PIMAGE_DOS_HEADER pDosHeader = nullptr;
	PIMAGE_NT_HEADERS pNtHeader = nullptr;
	PIMAGE_SECTION_HEADER pSectionHeader = nullptr;
	PIMAGE_BASE_RELOCATION pBaseRelocation = nullptr;
	DWORD dwIncerment = 0;
	DWORD dwFileSize = 0;
	DWORD dwRetBytes = 0;
	char* pBuf = nullptr;
	char* pFileBuf = nullptr;


	if (!DeviceIoControl(g_hDevice, READ_FILE, szPath, dwPathSize, NULL, 0, &dwRetBytes, NULL))
	{
		fmt.Format(L"[testR3] LoadPE: 获取文件大小失败\r\n");
		OutputDebugString(fmt);
		return NULL;
	}
	dwFileSize = dwRetBytes;
	pFileBuf = new char[dwFileSize];
	RtlZeroMemory(pFileBuf, dwFileSize);
	if (!DeviceIoControl(g_hDevice, READ_FILE, szPath, dwPathSize, pFileBuf, dwFileSize, &dwRetBytes, NULL))
	{
		fmt.Format(L"[testR3] LoadPE: 获取文件数据\r\n");
		OutputDebugString(fmt);
		delete[]pFileBuf;
		return NULL;
	}

	/*读取文件头*/
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuf;

	/*读取NT头*/
	pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuf + pDosHeader->e_lfanew);

	/*获取第一个节区头*/
	pSectionHeader = (PIMAGE_SECTION_HEADER)(pFileBuf + pDosHeader->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + pNtHeader->FileHeader.SizeOfOptionalHeader);

	/*申请缓冲区*/
	pBuf = (char*)VirtualAlloc(NULL, pNtHeader->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pBuf == nullptr)
	{
		fmt.Format(L"[testR3] LoadPE: VirtualAlloc Err, ErrCode=%d, SizeOfImage=%p\r\n", GetLastError(), pNtHeader->OptionalHeader.SizeOfImage);
		OutputDebugString(fmt);
		return NULL;
	}
	RtlZeroMemory(pBuf, pNtHeader->OptionalHeader.SizeOfImage);

	/*复制PE头*/
	memcpy_s(pBuf, pNtHeader->OptionalHeader.SizeOfHeaders, pFileBuf, pNtHeader->OptionalHeader.SizeOfHeaders);

	/*复制节区*/
	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		DWORD dwSize = pSectionHeader[i].SizeOfRawData;
		memcpy_s(pBuf + pSectionHeader[i].VirtualAddress, dwSize, pFileBuf + pSectionHeader[i].PointerToRawData, dwSize);

		if (pSectionHeader[i].Characteristics & IMAGE_SCN_MEM_EXECUTE &&
			((pSectionHeader[i].Characteristics & IMAGE_SCN_MEM_WRITE) == 0))
		{
			SECTION section = { 0 };
			memcpy_s(section.m_Name, 8, pSectionHeader[i].Name, 8);
			section.m_dwRVA = pSectionHeader[i].VirtualAddress;
			section.m_dwSize = pSectionHeader[i].SizeOfRawData;

			//有些时候IAT表会和其他节合在一起，所以判断一下
			if (section.m_dwRVA == pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress)
			{
				section.m_dwRVA += pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size;
			}
			pCheckList->AddTail(section);
		}
	}

	/*重定位修复*/
	dwIncerment = dwRelocationBase - pNtHeader->OptionalHeader.ImageBase;
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(pBuf + pNtHeader->OptionalHeader.DataDirectory[5].VirtualAddress);
	while (pBaseRelocation->SizeOfBlock != 0 && pBaseRelocation->VirtualAddress != 0)
	{
		DWORD RelocationNumber = (pBaseRelocation->SizeOfBlock - 8) / 2;
		for (int i = 0; i < RelocationNumber; i++)
		{
			WORD TypeOffset = *((WORD*)((char*)pBaseRelocation + 8) + i);
			if ((TypeOffset & 0xF000) == 0x3000)
			{
				TypeOffset = TypeOffset ^ 0x3000;
				DWORD dwOffset = pBaseRelocation->VirtualAddress + TypeOffset;
				DWORD dwAddr = 0;
				memcpy_s(&dwAddr, 4, pBuf + dwOffset, 4);
				dwAddr += dwIncerment;
				memcpy_s(pBuf + dwOffset, 4, &dwAddr, 4);
			}
		}
		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((char*)pBaseRelocation + pBaseRelocation->SizeOfBlock);
	}
	*pImgBase = (PVOID)pNtHeader->OptionalHeader.ImageBase;
	delete[]pFileBuf;
	return pBuf;
}

