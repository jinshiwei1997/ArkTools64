// CProcessTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CProcessTabDlg.h"
#include "afxdialogex.h"
#include "global.h"


// CProcessTabDlg 对话框

IMPLEMENT_DYNAMIC(CProcessTabDlg, CDialogEx)

CProcessTabDlg::CProcessTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_PROCESS, pParent)
{

}

CProcessTabDlg::~CProcessTabDlg()
{
}

void CProcessTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_PROC, m_lcProcess);
}


BEGIN_MESSAGE_MAP(CProcessTabDlg, CDialogEx)
	ON_NOTIFY(NM_RCLICK, LIST_PROC, &CProcessTabDlg::OnRclickListProc)
	ON_COMMAND(MENU_THREAD, &CProcessTabDlg::OnMenuThread)
	ON_COMMAND(MENU_PROCESS_EXIT, &CProcessTabDlg::OnMenuProcessExit)
	ON_COMMAND(MENU_PROCESS_MEM, &CProcessTabDlg::OnMenuProcessMem)
END_MESSAGE_MAP()


// CProcessTabDlg 消息处理程序


BOOL CProcessTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcProcess.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcProcess.InsertColumn(i++, L"进程名");
		m_lcProcess.InsertColumn(i++, L"进程ID");
		m_lcProcess.InsertColumn(i++, L"父进程ID");
		m_lcProcess.InsertColumn(i++, L"映像路径");
		m_lcProcess.InsertColumn(i++, L"EPROCESS");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 10;
		m_lcProcess.SetColumnWidth(i++, dwWidth * 3);
		m_lcProcess.SetColumnWidth(i++, dwWidth * 1);
		m_lcProcess.SetColumnWidth(i++, dwWidth * 1);
		m_lcProcess.SetColumnWidth(i++, dwWidth * 3);
		m_lcProcess.SetColumnWidth(i++, tabRect.Width() - dwWidth * 8);

		/*设置列表风格*/
		m_lcProcess.SetExtendedStyle(m_lcProcess.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CProcessTabDlg::InsertListItem()
{
	CString fmt;
	DWORD dwRetByte = 0;
	DWORD dwBufSize = 0;
	PROCESS_INFO* pBuf = nullptr;

	m_lcProcess.DeleteAllItems();		//清空列表

	//获取缓冲区大小
	if (!DeviceIoControl(g_hDevice, ENUM_PROCESS, NULL, 0, NULL, 0, &dwRetByte, NULL))
	{
		AfxMessageBox(TEXT("获取缓冲区大小失败！"));
		return;
	}
	
	//获取进程信息
	dwBufSize = dwRetByte;
	pBuf = (PROCESS_INFO*)new char[dwBufSize];
	RtlZeroMemory(pBuf, dwBufSize);
	if (!DeviceIoControl(g_hDevice, ENUM_PROCESS, NULL, 0, pBuf, dwBufSize, &dwRetByte, NULL))
	{
		AfxMessageBox(TEXT("获取进程信息失败！"));
		goto PROCESS_EXIT;
	}

	//输出信息
	for (DWORD i = 0; i < dwBufSize / sizeof(PROCESS_INFO); i++)
	{
		DWORD nSubItem = 1;
		m_lcProcess.InsertItem(i, pBuf[i].m_szName);
		fmt.Format(L"%d", pBuf[i].m_nPid);
		m_lcProcess.SetItemText(i, nSubItem++, fmt);
		m_lcProcess.SetItemData(i, (DWORD_PTR)pBuf[i].m_nPid);
		fmt.Format(L"%d", pBuf[i].m_nInheritedPid);
		m_lcProcess.SetItemText(i, nSubItem++, fmt);
		fmt = pBuf[i].m_szPatch;
		MSDos2DiskSymbol(fmt);
		m_lcProcess.SetItemText(i, nSubItem++, fmt);
		fmt.Format(L"0x%p", pBuf[i].m_pEPROCESS);
		m_lcProcess.SetItemText(i, nSubItem++, fmt);

	}

PROCESS_EXIT:
	if(pBuf != nullptr)
		delete[]pBuf;
	return;
}

void CProcessTabDlg::MSDos2DiskSymbol(CString& strPath)
{
	int n = strPath.Find(L"\\Device\\");
	if (n == -1 || n > 0)		//判断下标是否大于0是为了防止误伤名为"Device"的文件夹
	{
		return;
	}

	WCHAR ch = L'C';
	WCHAR szDisk[] = L"C:";
	WCHAR szBuf[MAX_PATH] = { 0 };
	for (; ch <= L'Z'; ch++)
	{
		szDisk[0] = ch;
		if (QueryDosDevice(szDisk, szBuf, MAX_PATH))
		{
			if (strPath.Replace(szBuf, szDisk))
			{
				break;
			}
		}
	}
}


void CProcessTabDlg::OnRclickListProc(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMItemActivate->iItem != -1)
	{
		m_dwCurIdx = pNMItemActivate->iItem;
		CMenu mnContext;
		CMenu* pmnSubMenu = nullptr;
		POINT pt;
		mnContext.LoadMenu(MENU_MAIN);
		pmnSubMenu = mnContext.GetSubMenu(1);
		GetCursorPos(&pt);
		pmnSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	}
	*pResult = 0;
}


void CProcessTabDlg::OnMenuThread()
{
	CThreadChildDlg Dlg;
	Dlg.m_hPid = (HANDLE)m_lcProcess.GetItemData(m_dwCurIdx);
	Dlg.DoModal();
}


void CProcessTabDlg::OnMenuProcessExit()
{
	// TODO: 在此添加命令处理程序代码
	DWORD dwRetBytes = 0;
	PROCESS_EXIT pe = { 0 };
	pe.m_hPid = (HANDLE)m_lcProcess.GetItemData(m_dwCurIdx);
	pe.m_pfnPspTerminateThreadByPointer = (PCHAR)g_pNtoskrnlBase + g_symNtkrnl.SymToRva(L"PspTerminateThreadByPointer");

	if (DeviceIoControl(g_hDevice, TERMINATE_PROCESS, &pe, sizeof(pe), NULL, 0, &dwRetBytes, NULL))
	{
		InsertListItem();
	}
	else
	{
		AfxMessageBox(L"进程结束失败");
	}
}


void CProcessTabDlg::OnMenuProcessMem()
{
	// TODO: 在此添加命令处理程序代码
	CMemoryChildDlg Dlg;
	Dlg.m_hPid = (HANDLE)m_lcProcess.GetItemData(m_dwCurIdx);
	Dlg.DoModal();
}
