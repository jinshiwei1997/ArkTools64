// CMemoryChildDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CMemoryChildDlg.h"
#include "afxdialogex.h"
#include "global.h"


// CMemoryChildDlg 对话框

IMPLEMENT_DYNAMIC(CMemoryChildDlg, CDialogEx)

CMemoryChildDlg::CMemoryChildDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CHILD_DLG_MEMORY, pParent)
{

}

CMemoryChildDlg::~CMemoryChildDlg()
{
}

void CMemoryChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_READ_ADDR, m_editReadAddr);
	DDX_Control(pDX, EDIT_WRITE_ADDR, m_editWriteAddr);
	DDX_Control(pDX, EDIT_WRITE_DATA, m_editWriteData);
	DDX_Control(pDX, CB_WRITE_DATA, m_cbWriteType);
	DDX_Control(pDX, LIST_MEMORY, m_lcData);
}


BEGIN_MESSAGE_MAP(CMemoryChildDlg, CDialogEx)
	ON_BN_CLICKED(BTN_READ_MEMORY, &CMemoryChildDlg::OnBnClickedReadMemory)
	ON_BN_CLICKED(BTN_WRITE_MEMORY, &CMemoryChildDlg::OnBnClickedWriteMemory)
END_MESSAGE_MAP()


// CMemoryChildDlg 消息处理程序


BOOL CMemoryChildDlg::OnInitDialog()
{
	CString fmt;
	CRect rect;
	CDialogEx::OnInitDialog();
	
	//插入标题
	m_lcData.InsertColumn(0, L"Offset");
	for (int i = 1; i < 17; i++)
	{
		fmt.Format(L"%X", i - 1);
		m_lcData.InsertColumn(i, fmt);
	}

	//设置宽度
	m_lcData.GetWindowRect(&rect);
	int nWidth = rect.Width() / 20;
	m_lcData.SetColumnWidth(0, rect.Width() - nWidth * 16 - 4);
	for (int i = 1; i < 17; i++)
	{
		m_lcData.SetColumnWidth(i, nWidth);
	}

	//设置风格
	m_lcData.SetExtendedStyle(m_lcData.GetExtendedStyle() | LVS_EX_GRIDLINES);

	m_cbWriteType.InsertString(0, L"1 Byte");
	m_cbWriteType.SetItemData(0, 2);
	m_cbWriteType.InsertString(1, L"4 Byte");
	m_cbWriteType.SetItemData(1, 8);
	m_cbWriteType.InsertString(2, L"8 Byte");
	m_cbWriteType.SetItemData(2, 16);

	m_cbWriteType.SetCurSel(2);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CMemoryChildDlg::PreTranslateMessage(MSG* pMsg)
{

	if ((WM_CHAR == pMsg->message) && ((::GetDlgCtrlID(pMsg->hwnd) == EDIT_READ_ADDR)||
									   (::GetDlgCtrlID(pMsg->hwnd) == EDIT_WRITE_ADDR)||
									   (::GetDlgCtrlID(pMsg->hwnd) == EDIT_WRITE_DATA)))
	{
		if (pMsg->wParam == VK_BACK)//相应back键
			return CDialog::PreTranslateMessage(pMsg);
		TCHAR ch = (TCHAR)pMsg->wParam;
		if ((ch >= '0' && ch <= '9'))
			return CDialog::PreTranslateMessage(pMsg);
		if ((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
			return CDialog::PreTranslateMessage(pMsg);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMemoryChildDlg::InsertListItem()
{
	CString fmt;
	TARGET_PROC target = { 0 };
	UCHAR pData[0x100] = { 0 };
	SIZE_T Address = 0;
	DWORD dwRetBytes = 0;

	m_editReadAddr.GetWindowText(fmt);
	if (fmt.IsEmpty())
	{
		AfxMessageBox(L"输入为空");
		return;
	}
	if (fmt.GetLength() > ADDR_LENGTH)
	{
		AfxMessageBox(L"超出限制");
		return;
	}

	m_lcData.DeleteAllItems();
#ifdef _WIN64
	Address = wcstoull(fmt, NULL, 16);
#else
	Address = wcstoul(fmt, NULL, 16);
#endif
	Address = Address & ~0xF;
	target.m_hHandle = m_hPid;
	target.m_TargetAddres = (PVOID)Address;

	if (!DeviceIoControl(g_hDevice, READ_PROC_MEMORY, &target, sizeof(target), pData, sizeof(pData), &dwRetBytes, NULL))
	{
		AfxMessageBox(L"读取失败");
		return;
	}

	for (DWORD i = 0; i < 16; i++)
	{
		fmt.Format(L"0x%p", Address);
		m_lcData.InsertItem(i, fmt);
		for (int j = 0; j < 16; j++)
		{
			fmt.Format(L"%02X", *(pData + i * 16 + j));
			m_lcData.SetItemText(i, j + 1, fmt);
		}
		Address += 16;
	}
}

void CMemoryChildDlg::WriteData()
{
	CString fmt;
	CString strAddr;
	CString strData;
	TARGET_PROC target = { 0 };
	UCHAR pData[0x100] = { 0 };
	SIZE_T Address = 0;
	unsigned __int64 nData = 0;
	DWORD dwWriteSize = 0;
	DWORD dwRetBytes = 0;

	m_editWriteAddr.GetWindowText(strAddr);
	m_editWriteData.GetWindowText(strData);
	if (strAddr.IsEmpty() || strData.IsEmpty())
	{
		AfxMessageBox(L"输入为空");
		return;
	}
	if (strAddr.GetLength() > ADDR_LENGTH)
	{
		AfxMessageBox(L"写入地址超出限制");
		return;
	}
	dwWriteSize = m_cbWriteType.GetItemData(m_cbWriteType.GetCurSel());
	if (strData.GetLength() > dwWriteSize)
	{
		AfxMessageBox(L"写入数据超出限制");
		return;
	}

#ifdef _WIN64
	Address = wcstoull(strAddr, NULL, 16);
#else
	Address = wcstoul(fmt, NULL, 16);
#endif
	nData = wcstoull(strData, NULL, 16);

	target.m_hHandle = m_hPid;
	target.m_TargetAddres = (PVOID)Address;
	target.m_nData = nData;
	//fmt.Format(L"%p", nData);
	//AfxMessageBox(fmt);
	if (!DeviceIoControl(g_hDevice, WRITE_PROC_MEMORY, &target, sizeof(target), pData, dwWriteSize / 2, &dwRetBytes, NULL))
	{
		AfxMessageBox(L"写入失败");
		return;
	}
	InsertListItem();
}


void CMemoryChildDlg::OnBnClickedReadMemory()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertListItem();
}

void CMemoryChildDlg::OnBnClickedWriteMemory()
{
	// TODO: 在此添加控件通知处理程序代码
	WriteData();
}
