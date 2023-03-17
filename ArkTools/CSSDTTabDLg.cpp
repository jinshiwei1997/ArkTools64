// CSSDTTabDLg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CSSDTTabDLg.h"
#include "afxdialogex.h"
#include "global.h"


// CSSDTTabDLg 对话框

IMPLEMENT_DYNAMIC(CSSDTTabDLg, CDialogEx)

CSSDTTabDLg::CSSDTTabDLg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_SSDT, pParent)
{

}

CSSDTTabDLg::~CSSDTTabDLg()
{
}

void CSSDTTabDLg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_SSDT, m_lcSSDT);
}


BEGIN_MESSAGE_MAP(CSSDTTabDLg, CDialogEx)
END_MESSAGE_MAP()


// CSSDTTabDLg 消息处理程序


BOOL CSSDTTabDLg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		/*设置列表风格*/
		m_lcSSDT.SetExtendedStyle(m_lcSSDT.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcSSDT.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcSSDT.InsertColumn(i++, L"序号");
		m_lcSSDT.InsertColumn(i++, L"函数名");
		m_lcSSDT.InsertColumn(i++, L"当前函数地址");
		m_lcSSDT.InsertColumn(i++, L"HOOK");
		m_lcSSDT.InsertColumn(i++, L"原始函数地址");
		m_lcSSDT.InsertColumn(i++, L"当前函数模块地址");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 6;
		m_lcSSDT.SetColumnWidth(i++, dwWidth * 1 - 80);
		m_lcSSDT.SetColumnWidth(i++, dwWidth * 1 + 70);
		m_lcSSDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcSSDT.SetColumnWidth(i++, dwWidth * 1 - 80);
		m_lcSSDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcSSDT.SetColumnWidth(i++, tabRect.Width() - dwWidth * 5 + 90);

		m_dwNtRVA = g_symNtkrnl.SymToRva(L"KiServiceTable");
		m_dwGdiRVA = g_symWin32k.SymToRva(L"W32pServiceTable");

	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSSDTTabDLg::InsertListItem()
{
	KSERVICE_TABLE_DESCRIPTOR ssdt[2] = { 0 };
	DRIVER_INFO* pDivInfo = nullptr;
	LONG* pFunctionAry = nullptr;
	DWORD dwRetByte = 0;
	DWORD dwDivInfoSize = 0;
	PVOID pKernelBase = 0;
	PVOID pWin32kBase = 0;
	PVOID pSSDTAddr = 0;
	DWORD dwRVA = 0;
	CString strKernel = L"ntoskrnl.exe";
	CString strWin32k = L"win32k.sys";
	CString fmt;
	DWORD dwListCnt = 0;

	m_lcSSDT.DeleteAllItems();

	//获取缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER, NULL, 0, NULL, 0, &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取驱动模块信息所需缓冲区大小失败!");
		return;
	}

	//获取信息
	dwDivInfoSize = dwRetByte;
	pDivInfo = (DRIVER_INFO*)new char[dwDivInfoSize];
	RtlZeroMemory(pDivInfo, dwDivInfoSize);
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER, NULL, 0, pDivInfo, dwDivInfoSize, &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取驱动模块信息失败!");
		delete[]pDivInfo;
		return;
	}

	for (DWORD i = 0; i < dwDivInfoSize / sizeof(DRIVER_INFO); i++)
	{
		if (pKernelBase != NULL && pWin32kBase != NULL)
		{
			break;
		}
		if (strKernel == pDivInfo[i].m_szName)
		{
			pKernelBase = pDivInfo[i].m_nBase;
		}
		if (strWin32k == pDivInfo[i].m_szName)
		{
			pWin32kBase = pDivInfo[i].m_nBase;
		}
	}

	//通过pdb获取ssdt shadow偏移
	dwRVA = g_symNtkrnl.SymToRva(L"KeServiceDescriptorTableShadow");
	if (dwRVA == 0)
	{
		AfxMessageBox(L"获取SSDT Shadow基址失败!");
		goto SSDT_EXIT;
	}

	//获取内存中ssdt shadow表信息
	pSSDTAddr = (PUCHAR)pKernelBase + dwRVA;
	if (!DeviceIoControl(g_hDevice, READ_SYS_MEMORY, &pSSDTAddr, sizeof(pSSDTAddr), &ssdt, sizeof(ssdt), &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取SSDT Shadow信息失败!");
		goto SSDT_EXIT;
	}

	//获取函数列表
	pFunctionAry = (LONG*)new char[ssdt[0].Limit * sizeof(ULONG)];
	RtlZeroMemory(pFunctionAry, ssdt[0].Limit * sizeof(ULONG));
	if (!DeviceIoControl(g_hDevice, READ_SYS_MEMORY, &ssdt[0].Base, sizeof(ssdt[0].Base), pFunctionAry, ssdt[0].Limit * sizeof(ULONG), &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取函数列表失败!");
		delete[]pFunctionAry;
		goto SSDT_EXIT;
	}
#ifdef _WIN64
	for (DWORD i = 0; i < ssdt[0].Limit; i++)
	{
		DWORD dw = 1;
		PVOID pOriginalAddr = GetOriginalSSDTFunction(i, FALSE);
		PVOID pCurAddr = (PCHAR)ssdt[0].Base + ((LONG)pFunctionAry[i] >> 4);
		CString strCurPath = L"C:\\Windows\\System32\\ntkrnlpa.exe";
		dwRVA = (SIZE_T)pOriginalAddr - (SIZE_T)pKernelBase;
		fmt.Format(L"%d", i);
		m_lcSSDT.InsertItem(i, fmt);			//序号
		m_lcSSDT.SetItemText(i, dw++, g_symNtkrnl.RvaToSym(dwRVA).c_str());	//函数名
		fmt.Format(L"0x%p", pCurAddr);
		m_lcSSDT.SetItemText(i, dw++, fmt);		//当前函数地址

		if (pOriginalAddr == pCurAddr)
		{
			fmt.Format(L"-");
		}
		else
		{
			fmt.Format(L"SSDT HOOK");
			for (DWORD j = 0; j < dwDivInfoSize / sizeof(DRIVER_INFO); j++)
			{
				if (pCurAddr >= pDivInfo[j].m_nBase && pCurAddr < (PVOID)((PCHAR)pDivInfo[j].m_nBase + pDivInfo[j].m_nSize))
				{
					strCurPath = pDivInfo[j].m_szPatch;
					break;
				}
			}
		}
		m_lcSSDT.SetItemText(i, dw++, fmt);		//Hook类型
		fmt.Format(L"0x%p", pOriginalAddr);
		m_lcSSDT.SetItemText(i, dw++, fmt);		//原始函数地址
		m_lcSSDT.SetItemText(i, dw++, strCurPath);		//当前函数所在模块
	}
	delete[]pFunctionAry;
	pFunctionAry = NULL;
#endif

	//获取函数列表
	pFunctionAry = (LONG*)new char[ssdt[1].Limit * sizeof(ULONG)];
	RtlZeroMemory(pFunctionAry, ssdt[1].Limit * sizeof(ULONG));
	if (!DeviceIoControl(g_hDevice, READ_SYS_MEMORY, &ssdt[1].Base, sizeof(ssdt[1].Base), pFunctionAry, ssdt[1].Limit * sizeof(ULONG), &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取函数列表(GDI)失败!");
		delete[]pFunctionAry;
		goto SSDT_EXIT;
	}
	dwListCnt = m_lcSSDT.GetItemCount();
#ifdef _WIN64
	for (DWORD i = 0; i < ssdt[1].Limit; i++)
	{
		DWORD dw = 1;
		PVOID pOriginalAddr = GetOriginalSSDTFunction(i, TRUE);
		PVOID pCurAddr = (PCHAR)ssdt[1].Base + (pFunctionAry[i] >> 4);
		CString strCurPath = L"C:\\Windows\\System32\\win32k.sys";
		dwRVA = (SIZE_T)pCurAddr - (SIZE_T)pWin32kBase;
		fmt.Format(L"%d", i);
		m_lcSSDT.InsertItem(i + dwListCnt, fmt);			//序号
		m_lcSSDT.SetItemText(i + dwListCnt, dw++, g_symWin32k.RvaToSym(dwRVA).c_str());	//函数名
		fmt.Format(L"0x%p", pCurAddr);
		m_lcSSDT.SetItemText(i + dwListCnt, dw++, fmt);		//当前函数地址

		if (pOriginalAddr == pCurAddr)
		{
			fmt.Format(L"-");
		}
		else
		{
			fmt.Format(L"SSDT HOOK");
			for (DWORD j = 0; j < dwDivInfoSize / sizeof(DRIVER_INFO); j++)
			{
				if (pCurAddr >= pDivInfo[j].m_nBase && pCurAddr < (PVOID)((PCHAR)pDivInfo[j].m_nBase + pDivInfo[j].m_nSize))
				{
					strCurPath = pDivInfo[j].m_szPatch;
					break;
				}
			}
		}
		m_lcSSDT.SetItemText(i + dwListCnt, dw++, fmt);		//Hook类型
		fmt.Format(L"0x%p", pOriginalAddr);
		m_lcSSDT.SetItemText(i + dwListCnt, dw++, fmt);		//原始函数地址
		m_lcSSDT.SetItemText(i + dwListCnt, dw++, strCurPath);		//当前函数所在模块
	}
	delete[]pFunctionAry;
	pFunctionAry = NULL;
#endif

SSDT_EXIT:
	delete[]pDivInfo;
}

PVOID CSSDTTabDLg::GetOriginalSSDTFunction(DWORD dwIdx, BOOL bIsGdi)
{
	//ssdt的函数表在文件中还是以8字节存放，到了内存后才跟以前一样是4字节
	CString fmt;
	PVOID pOriginalSSDTFunction = NULL;
	PVOID* pSSDT = nullptr;

	if ((g_pOridinalNtoskrnl == nullptr && bIsGdi == FALSE) || (g_pOridinalWin32k == nullptr && bIsGdi == TRUE))
	{
		fmt.Format(L"[testR3] GetOriginalSSDTFunction: 内核模块加载失败, g_pNtoskrnl=%p, g_pWin32k=%p\r\n", g_pOridinalNtoskrnl, g_pOridinalWin32k);
		OutputDebugString(fmt);
		return NULL;
	}

	if ((m_dwNtRVA == -1 && bIsGdi == FALSE) || (m_dwGdiRVA == -1 && bIsGdi == TRUE))
	{
		fmt.Format(L"[testR3] GetOriginalSSDTFunction: SymToRva Err, dwNtRVA=0x%08X, dwGdiRVA=0x%08X\r\n", m_dwNtRVA, m_dwGdiRVA);
		OutputDebugString(fmt);
		return NULL;
	}

	if (!bIsGdi)
	{
		pSSDT = (PVOID*)((char*)g_pOridinalNtoskrnl + m_dwNtRVA);
#ifdef _WIN64
		pOriginalSSDTFunction = (PVOID)((PCHAR)pSSDT[dwIdx] - (SIZE_T)g_pNtoskrnlImgBase + (SIZE_T)g_pNtoskrnlBase);
#else
		pOriginalSSDTFunction = pSSDT[dwIdx]
#endif
	}
	else
	{
		pSSDT = (PVOID*)((char*)g_pOridinalWin32k + m_dwGdiRVA);
#ifdef _WIN64
		pOriginalSSDTFunction = (PVOID)((PCHAR)pSSDT[dwIdx] - (SIZE_T)g_pWin32kImgBase + (SIZE_T)g_pWin32kBase);
#else
		pOriginalSSDTFunction = pSSDT[dwIdx]
#endif
	}

	return pOriginalSSDTFunction;
}
