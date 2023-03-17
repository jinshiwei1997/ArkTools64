// CGDTDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CGDTDlg.h"
#include "afxdialogex.h"


// CGDTDlg 对话框

IMPLEMENT_DYNAMIC(CGDTDlg, CDialogEx)

CGDTDlg::CGDTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_GDT, pParent)
{

}

CGDTDlg::~CGDTDlg()
{
}

void CGDTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_GDT, m_lcGDT);
}


BEGIN_MESSAGE_MAP(CGDTDlg, CDialogEx)
END_MESSAGE_MAP()


// CGDTDlg 消息处理程序


BOOL CGDTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		/*设置列表风格*/
		m_lcGDT.SetExtendedStyle(m_lcGDT.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);
		m_lcGDT.MoveWindow(tabRect);

		/*标题*/
		DWORD i = 0;
		m_lcGDT.InsertColumn(i++, L"CPU序号");
		m_lcGDT.InsertColumn(i++, L"下标");
		m_lcGDT.InsertColumn(i++, L"基址");
		m_lcGDT.InsertColumn(i++, L"界限");
		m_lcGDT.InsertColumn(i++, L"颗粒度");
		m_lcGDT.InsertColumn(i++, L"段特权级");
		m_lcGDT.InsertColumn(i++, L"类型");

		/*设置列表每列标题宽度*/
		i = 0;
		DWORD dwWidth = tabRect.Width() / 7;
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1 - 60);
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcGDT.SetColumnWidth(i++, dwWidth * 1);
		m_lcGDT.SetColumnWidth(i++, tabRect.Width() - dwWidth * 6 + 60);

	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

#ifdef _WIN64
#define SZFORMAT_BASE	TEXT("0x%08X%02X%02X%04X")
#define SZFORMAT_LIMIT1 TEXT("0x00000000%01X%04XFFF")
#define SZFORMAT_LIMIT2 TEXT("0x00000000000%01X%04X")
#else
#define SZFORMAT_BASE	TEXT("0x%02X%02X%04X")
#define SZFORMAT_LIMIT1 TEXT("0x%01X%04XFFF")
#define SZFORMAT_LIMIT2 TEXT("0x000%01X%04X")
#endif

void CGDTDlg::InsertListItem()
{
	SYSTEM_INFO si = { 0 };
	GetSystemInfo(&si);
	DWORD dwShift = 1;

	m_lcGDT.DeleteAllItems();

	for (DWORD k = 0; k < si.dwNumberOfProcessors; k++)
	{
		GDT gdt = { 0 };
		DWORD dwRetByte = 0;
		DWORD dwSize = 0;
		SEG_DESCRIPTOR* psd = nullptr;
		SetProcessAffinityMask(GetCurrentProcess(), dwShift);
		_sgdt(&gdt);
		dwSize = gdt.Size + 1;
		psd = (SEG_DESCRIPTOR*)new char[dwSize];
		RtlZeroMemory(psd, dwSize);
		if (!DeviceIoControl(g_hDevice, ENUM_GDT, &gdt, sizeof(gdt), psd, dwSize, &dwRetByte, NULL))
		{
			AfxMessageBox(L"获取GDT信息失败!");
			return;
		}

		for (size_t i = 0; i < dwSize / 8; i++)
		{
			if (psd[i].P == 1)
			{
				CString fmt;
				size_t j = 1;
				DWORD dwIdx = m_lcGDT.GetItemCount();
				fmt.Format(L"%d", k + 1);
				m_lcGDT.InsertItem(dwIdx, fmt);
				fmt.Format(L"%01X", i);
				m_lcGDT.SetItemText(dwIdx, j++, fmt);
#ifdef _WIN64
				DWORD dwHigh = 0;
				if (psd[i].Base2 >= 0x80)
					dwHigh = 0xFFFFFFFF;
				fmt.Format(SZFORMAT_BASE, dwHigh, psd[i].Base2, psd[i].Base1, psd[i].Base0);
#else
				fmt.Format(SZFORMAT_BASE, psd[i].Base2, psd[i].Base1, psd[i].Base0);
#endif // _WIN64
				
				m_lcGDT.SetItemText(dwIdx, j++, fmt);
				if (psd[i].G == 1)
				{
					fmt.Format(SZFORMAT_LIMIT1, psd[i].Limit1, psd[i].Limit0);
					m_lcGDT.SetItemText(dwIdx, j++, fmt);
					m_lcGDT.SetItemText(dwIdx, j++, L"Page");
				}
				else
				{
					fmt.Format(SZFORMAT_LIMIT2, psd[i].Limit1, psd[i].Limit0);
					m_lcGDT.SetItemText(dwIdx, j++, fmt);
					m_lcGDT.SetItemText(dwIdx, j++, L"Byte");
				}
				fmt.Format(L"%d", psd[i].DPL);
				m_lcGDT.SetItemText(dwIdx, j++, fmt);
				m_lcGDT.SetItemText(dwIdx, j++, GetTypeString(psd[i]));
			}

		}

		if (psd != nullptr)
		{
			delete[]psd;
		}
		Sleep(1);
		dwShift <<= 1;
	}
}

CString CGDTDlg::GetTypeString(SEG_DESCRIPTOR& sd)
{
	CString fmt;
	if (sd.S == 0)		//系统段描述符
	{
		switch (sd.Type)
		{
		case 2:
			fmt = L"LDT";				//ldt表
			break;
		case 5:
			fmt = L"Task Gate";			//任务段
			break;
		case 9:
			fmt = L"TSS (Avaliable)";	//TSS (空闲)
			break;
		case 11:
			fmt = L"TSS (Busy)";		//TSS (繁忙)
			break;
		case 12:
			fmt = L"Call Gate";			//调用门
			break;
		case 14:
			fmt = L"Interrupt Gate";	//中断门
			break;
		case 15:
			fmt = L"Trap Gate";			//陷阱门
			break;
		}
	}
	else
	{
		switch (sd.Type)
		{
		case 0:
			fmt = L"Data R";
			break;
		case 1:
			fmt = L"Data R Accessed";
			break;
		case 2:
			fmt = L"Data RW";
			break;
		case 3:
			fmt = L"Data RW Accessed";
			break;
		case 4:
			fmt = L"Data R EX";
			break;
		case 5:
			fmt = L"Data R EX Accessed";
			break;
		case 6:
			fmt = L"Data RW EX";
			break;
		case 7:
			fmt = L"Data RW EX Accessed";
			break;
		case 8:
			fmt = L"Code E";
			break;
		case 9:
			fmt = L"Code E Accessed";
			break;
		case 10:
			fmt = L"Code ER";
			break;
		case 11:
			fmt = L"Code ER Accessed";
			break;
		case 12:
			fmt = L"Code E Conforming";
			break;
		case 13:
			fmt = L"Code E Conforming Accessed";
			break;
		case 14:
			fmt = L"Code E Conforming";
			break;
		case 15:
			fmt = L"Code ER Conforming Accessed";
			break;
		}
	}


	return fmt;
}
