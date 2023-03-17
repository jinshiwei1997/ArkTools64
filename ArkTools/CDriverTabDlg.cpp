// CDriverTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CDriverTabDlg.h"
#include "afxdialogex.h"

#include "global.h"

// CDriverTabDlg 对话框

IMPLEMENT_DYNAMIC(CDriverTabDlg, CDialogEx)

CDriverTabDlg::CDriverTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_DRIVER, pParent)
{

}

CDriverTabDlg::~CDriverTabDlg()
{
}

void CDriverTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_DRIVER, m_lcDriver);
}


BEGIN_MESSAGE_MAP(CDriverTabDlg, CDialogEx)
END_MESSAGE_MAP()


// CDriverTabDlg 消息处理程序


BOOL CDriverTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcDriver.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcDriver.InsertColumn(i++, L"驱动名");
		m_lcDriver.InsertColumn(i++, L"基地址");
		m_lcDriver.InsertColumn(i++, L"大小");
		m_lcDriver.InsertColumn(i++, L"驱动对象");
		m_lcDriver.InsertColumn(i++, L"驱动路径");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 6;
		m_lcDriver.SetColumnWidth(i++, dwWidth);
		m_lcDriver.SetColumnWidth(i++, dwWidth);
		m_lcDriver.SetColumnWidth(i++, dwWidth);
		m_lcDriver.SetColumnWidth(i++, dwWidth);
		m_lcDriver.SetColumnWidth(i++, dwWidth * 2);

		/*设置列表风格*/
		m_lcDriver.SetExtendedStyle(m_lcDriver.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDriverTabDlg::InsertListItem()
{
	DRIVER_INFO* pDivInfo = nullptr;
	DWORD dwRetByte = 0;
	DWORD dwReadSize = 0;
	CString fmt;

	m_lcDriver.DeleteAllItems();

	//获取缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER, NULL, 0, NULL, 0, &dwRetByte, NULL))
	{
		AfxMessageBox(L"获取驱动信息所需缓冲区大小失败!");
		return;
	}

	//获取信息
	pDivInfo = (DRIVER_INFO*)new char[dwRetByte];
	RtlZeroMemory(pDivInfo, dwRetByte);
	if (!DeviceIoControl(g_hDevice, ENUM_DRIVER,
		NULL, 0,
		pDivInfo, dwRetByte,
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"获取驱动信息所需缓冲区大小失败!");
		goto DRIVER_EXIT;
	}

	for (int i = 0; i < dwRetByte / sizeof(DRIVER_INFO); i++)
	{
		int n = 1;
		m_lcDriver.InsertItem(i, pDivInfo[i].m_szName);
		fmt.Format(L"0x%p", pDivInfo[i].m_nBase);
		m_lcDriver.SetItemText(i, n++, fmt);
		fmt.Format(L"%d", pDivInfo[i].m_nSize);
		m_lcDriver.SetItemText(i, n++, fmt);
		fmt.Format(L"0");
		m_lcDriver.SetItemText(i, n++, fmt);

		fmt.Format(L"%s", pDivInfo[i].m_szPatch);
		fmt.Replace(_T("\\SystemRoot"), _T("C:\\Windows"));
		m_lcDriver.SetItemText(i, n++, fmt);
	}

DRIVER_EXIT:
	delete[]pDivInfo;
	return;
}
