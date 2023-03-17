// CSysCallBackTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CSysCallBackTabDlg.h"
#include "afxdialogex.h"
#include "global.h"


// CSysCallBackTabDlg 对话框

IMPLEMENT_DYNAMIC(CSysCallBackTabDlg, CDialogEx)

CSysCallBackTabDlg::CSysCallBackTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_SYSCALLBACK, pParent)
{

}

CSysCallBackTabDlg::~CSysCallBackTabDlg()
{
}

void CSysCallBackTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_CALLBACK, m_lcCallback);
}


BEGIN_MESSAGE_MAP(CSysCallBackTabDlg, CDialogEx)
END_MESSAGE_MAP()


// CSysCallBackTabDlg 消息处理程序


BOOL CSysCallBackTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcCallback.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcCallback.InsertColumn(i++, L"回调入口");
		m_lcCallback.InsertColumn(i++, L"类型");
		m_lcCallback.InsertColumn(i++, L"模块路径");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 4;
		m_lcCallback.SetColumnWidth(i++, dwWidth);
		m_lcCallback.SetColumnWidth(i++, dwWidth);
		m_lcCallback.SetColumnWidth(i++, tabRect.Width() - dwWidth * 2);

		/*设置列表风格*/
		m_lcCallback.SetExtendedStyle(m_lcCallback.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		/*获取回调数组基址*/
		m_pPspCreateProcessNotifyRoutine = (PCHAR)g_pNtoskrnlBase + g_symNtkrnl.SymToRva(L"PspCreateProcessNotifyRoutine");
		m_pPspCreateThreadNotifyRoutine = (PCHAR)g_pNtoskrnlBase + g_symNtkrnl.SymToRva(L"PspCreateThreadNotifyRoutine");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSysCallBackTabDlg::InsertListItem()
{
	CString fmt;
	PVOID CreateProcessNotifyRoutine[PSP_MAX_CREATE_PROCESS_NOTIFY] = { 0 };
	PVOID CreateThreadNotifyRoutine[PSP_MAX_CREATE_THREAD_NOTIFY] = { 0 };
	SYSTEM_CALLBACK sc = { 0 };
	DRIVER_INFO* pDivInfo = nullptr;
	DWORD dwDivInfoSize = 0;
	DWORD dwRetBytes = 0;
	DWORD dwItemCnt = 0;

	m_lcCallback.DeleteAllItems();

	//获取进程回调信息
	sc.m_uCount = PSP_MAX_CREATE_PROCESS_NOTIFY;
	sc.m_pNotifyRoutine = m_pPspCreateProcessNotifyRoutine;
	if (!DeviceIoControl(g_hDevice, ENUM_SYSCALLBACK, &sc, sizeof(sc),
		CreateProcessNotifyRoutine, sizeof(CreateProcessNotifyRoutine), &dwRetBytes, NULL))
	{
		AfxMessageBox(L"获取进程回调信息失败!");
		return;
	}

	//获取线程回调信息
	sc.m_uCount = PSP_MAX_CREATE_THREAD_NOTIFY;
	sc.m_pNotifyRoutine = m_pPspCreateThreadNotifyRoutine;
	if (!DeviceIoControl(g_hDevice, ENUM_SYSCALLBACK, &sc, sizeof(sc),
		CreateThreadNotifyRoutine, sizeof(CreateThreadNotifyRoutine), &dwRetBytes, NULL))
	{
		AfxMessageBox(L"获取线程回调信息失败!");
		return;
	}

	//获取驱动模块信息缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER, NULL, 0, NULL, 0, &dwRetBytes, NULL))
	{
		AfxMessageBox(L"获取驱动信息所需缓冲区大小失败!");
		return;
	}
	dwDivInfoSize = dwRetBytes;
	//获取驱动模块信息
	pDivInfo = (DRIVER_INFO*)new char[dwDivInfoSize];
	RtlZeroMemory(pDivInfo, dwDivInfoSize);
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER,
		NULL, 0,
		pDivInfo, dwDivInfoSize,
		&dwRetBytes, NULL))
	{
		AfxMessageBox(L"获取驱动信息所需缓冲区大小失败!");
		goto CALLBACK_EXIT;
	}


	//输出
	for (DWORD i = 0; i < PSP_MAX_CREATE_PROCESS_NOTIFY; i++)
	{
		if (CreateProcessNotifyRoutine[i] != NULL)
		{
			fmt.Format(L"0x%p", CreateProcessNotifyRoutine[i]);
			m_lcCallback.InsertItem(dwItemCnt, fmt);
			m_lcCallback.SetItemText(dwItemCnt, 1, L"CreateProcess");
			for (DWORD j = 0; j < dwDivInfoSize / sizeof(DRIVER_INFO); j++)
			{
				if (CreateProcessNotifyRoutine[i] >= pDivInfo[j].m_nBase && 
					CreateProcessNotifyRoutine[i] < (PVOID)((PCHAR)pDivInfo[j].m_nBase + pDivInfo[j].m_nSize))
				{
					fmt = pDivInfo[j].m_szPatch;
					break;
				}
			}
			m_lcCallback.SetItemText(dwItemCnt, 2, fmt);
			dwItemCnt++;
		}
	}
	for (DWORD i = 0; i < PSP_MAX_CREATE_THREAD_NOTIFY; i++)
	{
		if (CreateThreadNotifyRoutine[i] != NULL)
		{
			fmt.Format(L"0x%p", CreateThreadNotifyRoutine[i]);
			m_lcCallback.InsertItem(dwItemCnt, fmt);
			m_lcCallback.SetItemText(dwItemCnt, 1, L"CreateThread");
			for (DWORD j = 0; j < dwDivInfoSize / sizeof(DRIVER_INFO); j++)
			{
				if (CreateThreadNotifyRoutine[i] >= pDivInfo[j].m_nBase &&
					CreateThreadNotifyRoutine[i] < (PVOID)((PCHAR)pDivInfo[j].m_nBase + pDivInfo[j].m_nSize))
				{
					fmt = pDivInfo[j].m_szPatch;
					break;
				}
			}
			m_lcCallback.SetItemText(dwItemCnt, 2, fmt);
			dwItemCnt++;
		}
	}

CALLBACK_EXIT:
	if (pDivInfo != nullptr)
	{
		delete[]pDivInfo;
	}
	return;
}
