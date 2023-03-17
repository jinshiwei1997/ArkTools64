// CIDTDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CIDTDlg.h"
#include "afxdialogex.h"
#include "global.h"

// CIDTDlg 对话框

IMPLEMENT_DYNAMIC(CIDTDlg, CDialogEx)

CIDTDlg::CIDTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_IDT, pParent)
{

}

CIDTDlg::~CIDTDlg()
{
}

void CIDTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_IDT, m_lcIDT);
}


BEGIN_MESSAGE_MAP(CIDTDlg, CDialogEx)
END_MESSAGE_MAP()


// CIDTDlg 消息处理程序


BOOL CIDTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		/*设置列表风格*/
		m_lcIDT.SetExtendedStyle(m_lcIDT.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcIDT.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcIDT.InsertColumn(i++, L"CPU序号");
		m_lcIDT.InsertColumn(i++, L"下标");
		m_lcIDT.InsertColumn(i++, L"段选择子");
		m_lcIDT.InsertColumn(i++, L"函数名");
		m_lcIDT.InsertColumn(i++, L"函数地址");
		m_lcIDT.InsertColumn(i++, L"所在模块");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 7;
		m_lcIDT.SetColumnWidth(i++, dwWidth * 1 - 60);
		m_lcIDT.SetColumnWidth(i++, dwWidth * 1 - 60);
		m_lcIDT.SetColumnWidth(i++, dwWidth * 1 - 60);
		m_lcIDT.SetColumnWidth(i++, dwWidth * 1 + 60);
		m_lcIDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcIDT.SetColumnWidth(i++, tabRect.Width() - dwWidth * 5 + 120 - 4);

	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CIDTDlg::InsertListItem()
{
	CString fmt;
	SYSTEM_CALLBACK sc = { 0 };
	DRIVER_INFO* pDivInfo = nullptr;
	DWORD dwDivInfoSize = 0;
	DWORD dwRetBytes = 0;
	DWORD dwItemCnt = 0;
	SYSTEM_INFO si = { 0 };
	DWORD dwShift = 1;

	m_lcIDT.DeleteAllItems();
	GetSystemInfo(&si);


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
		if (pDivInfo != nullptr)
		{
			delete[]pDivInfo;
		}
		return;
	}


	for (DWORD k = 0; k < si.dwNumberOfProcessors; k++)
	{
		GDT idt = { 0 };
		DWORD dwSize = 0;
		GATE_DESCRIPTOR* psd = nullptr;
		SetProcessAffinityMask(GetCurrentProcess(), dwShift);
		__sidt(&idt);

		dwSize = idt.Size + 1;
		psd = (GATE_DESCRIPTOR*)new char[dwSize];
		RtlZeroMemory(psd, dwSize);
		if (!DeviceIoControl(g_hDevice, READ_SYS_MEMORY, &idt.SegDescriptor, sizeof(idt.SegDescriptor),
			psd, dwSize, &dwRetBytes, NULL))
		{
			AfxMessageBox(L"获取IDT信息失败!");
			if (psd != nullptr)
				delete[]psd;
			return;
		}

		for (DWORD i = 0; i < dwSize / sizeof(GATE_DESCRIPTOR); i++)
		{
			PVOID pAddress = 0;
			if (psd[i].P == 0)
			{
				continue;
			}
			SIZE_T tmp;
			pAddress = (PVOID)(psd[i].Offset0);
			tmp = psd[i].Offset1;
			pAddress = (PVOID)((SIZE_T)pAddress + tmp * 0x10000);
			tmp = psd[i].Offset2;
			pAddress = (PVOID)((SIZE_T)pAddress + tmp * 0x100000000);

			fmt.Format(L"%d", k + 1);
			m_lcIDT.InsertItem(dwItemCnt, fmt);				//CPU序号

			fmt.Format(L"%d", i);
			m_lcIDT.SetItemText(dwItemCnt, 1, fmt);			//下标

			fmt.Format(L"0x%04X", psd[i].Selector);
			m_lcIDT.SetItemText(dwItemCnt, 2, fmt);			//段选择子

			tmp = (SIZE_T)pAddress - (SIZE_T)g_pNtoskrnlBase;
			m_lcIDT.SetItemText(dwItemCnt, 3, g_symNtkrnl.RvaToSym(tmp).c_str());	//函数名

			fmt.Format(L"0x%p", pAddress);
			m_lcIDT.SetItemText(dwItemCnt, 4, fmt);			//函数地址

			for (DWORD j = 0; j < dwDivInfoSize / sizeof(DRIVER_INFO); j++)
			{
				if (pAddress >= pDivInfo[j].m_nBase &&
					pAddress < (PVOID)((PCHAR)pDivInfo[j].m_nBase + pDivInfo[j].m_nSize))
				{
					fmt = pDivInfo[j].m_szPatch;
					break;
				}
			}
			m_lcIDT.SetItemText(dwItemCnt, 5, fmt);			//所在模块
			dwItemCnt++;
		}

		Sleep(1);
		dwShift <<= 1;
		if (psd != nullptr)
			delete[]psd;
	}

	if (pDivInfo != nullptr)
		delete[]pDivInfo;
	return;
}
