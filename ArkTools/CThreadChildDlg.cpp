// CThreadChildDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CThreadChildDlg.h"
#include "afxdialogex.h"
#include "global.h"
#include <Psapi.h>

// CThreadChildDlg 对话框

IMPLEMENT_DYNAMIC(CThreadChildDlg, CDialogEx)

CThreadChildDlg::CThreadChildDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CHILD_DLG_THREAD, pParent)
{

}

CThreadChildDlg::~CThreadChildDlg()
{
}

void CThreadChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_THREAD, m_lcThread);
}


BEGIN_MESSAGE_MAP(CThreadChildDlg, CDialogEx)
END_MESSAGE_MAP()


// CThreadChildDlg 消息处理程序


BOOL CThreadChildDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetClientRect(&rect);
	m_lcThread.MoveWindow(rect);

	/*标题*/
	DWORD i = 0;
	m_lcThread.InsertColumn(i++, L"线程ID");
	m_lcThread.InsertColumn(i++, L"ETHREAD");
	m_lcThread.InsertColumn(i++, L"TEB");
	m_lcThread.InsertColumn(i++, L"优先级");
	m_lcThread.InsertColumn(i++, L"线程入口");
	m_lcThread.InsertColumn(i++, L"模块");
	m_lcThread.InsertColumn(i++, L"切换次数");
	m_lcThread.InsertColumn(i++, L"线程状态");

	/*设置列表每列标题宽度*/
	i = 0;
	DWORD dwWidth = rect.Width() / 8;
	m_lcThread.SetColumnWidth(i++, rect.Width() - dwWidth * 7);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);
	m_lcThread.SetColumnWidth(i++, dwWidth);

	/*设置列表风格*/
	m_lcThread.SetExtendedStyle(m_lcThread.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	InsertItem();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CThreadChildDlg::InsertItem()
{
	CString fmt;
	WCHAR szModule[260] = { 0 };
	THREAD_INFO* pBuffer = nullptr;
	DWORD dwBufSize = 0;
	DWORD dwRetBytes = 0;

	m_lcThread.DeleteAllItems();

	//获取缓冲区大小
	DeviceIoControl(g_hDevice, ENUM_THREAD, &m_hPid, sizeof(m_hPid), NULL, 0, &dwRetBytes, NULL);
	if (dwRetBytes == 0)
	{
		AfxMessageBox(L"获取线程信息所需缓冲区大小失败\r\n");
		return;
	}

	//申请缓冲区
	dwBufSize = dwRetBytes;
	pBuffer = (THREAD_INFO*)new char[dwBufSize];

	//获取信息
	if (!DeviceIoControl(g_hDevice, ENUM_THREAD, &m_hPid, sizeof(m_hPid), pBuffer, dwBufSize, &dwRetBytes, NULL))
	{
		AfxMessageBox(L"获取线程信息失败\r\n");
		delete[]pBuffer;
		return;
	}

	//输出
	for (DWORD i = 0; i < dwBufSize / sizeof(THREAD_INFO); i++)
	{
		DWORD j = 1;

		fmt.Format(L"%d", pBuffer[i].m_uCid);	
		m_lcThread.InsertItem(i, fmt);			//线程ID

		fmt.Format(L"0x%p", pBuffer[i].m_pETHREAD);
		m_lcThread.SetItemText(i, j++, fmt);	//ETHREAD

		fmt.Format(L"0x%p", pBuffer[i].m_TEB);
		m_lcThread.SetItemText(i, j++, fmt);	//TEB

		fmt.Format(L"%d", pBuffer[i].m_uPriority);
		m_lcThread.SetItemText(i, j++, fmt);	//优先级

		fmt.Format(L"0x%p", pBuffer[i].m_pStartAddr);
		m_lcThread.SetItemText(i, j++, fmt);	//入口地址

		::GetMappedFileName(GetCurrentProcess(), pBuffer[i].m_pStartAddr, szModule, sizeof(szModule));
		fmt = szModule;
		fmt.Delete(0, fmt.ReverseFind('\\') + 1);
		m_lcThread.SetItemText(i, j++, fmt);	//模块名

		fmt.Format(L"%d", pBuffer[i].m_uSwapCnt);
		m_lcThread.SetItemText(i, j++, fmt);	//切换次数

		m_lcThread.SetItemText(i, j++, StateToStr(pBuffer[i].m_uState));	//线程状态
	}
}

CString CThreadChildDlg::StateToStr(DWORD state)
{
	CString fmt;

	switch (state)
	{
	case 0:
		fmt = L"初始化";
		break;
	case 1:
		fmt = L"就绪";
		break;
	case 2:
		fmt = L"运行";
		break;
	case 3:
		fmt = L"备用";
		break;
	case 4:
		fmt = L"终止";
		break;
	case 5:
		fmt = L"等待";
		break;
	case 6:
		fmt = L"过渡";
		break;
	case 7:
		fmt = L"未知";
		break;
	}
	return fmt;
}
