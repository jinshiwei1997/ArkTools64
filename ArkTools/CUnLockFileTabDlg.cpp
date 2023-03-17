// CUnLockFileTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CUnLockFileTabDlg.h"
#include "afxdialogex.h"
#include "global.h"

// CUnLockFileTabDlg 对话框

IMPLEMENT_DYNAMIC(CUnLockFileTabDlg, CDialogEx)

CUnLockFileTabDlg::CUnLockFileTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_UNLOCK, pParent)
{

}

CUnLockFileTabDlg::~CUnLockFileTabDlg()
{
}

void CUnLockFileTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_PATH, m_editPath);
	DDX_Control(pDX, BTN_UNLOCK, m_btnUnlock);
	DDX_Control(pDX, BTN_SMASH, m_btnSmash);
	DDX_Control(pDX, TEXT_PATH, m_textPath);
}


BEGIN_MESSAGE_MAP(CUnLockFileTabDlg, CDialogEx)
	ON_BN_CLICKED(BTN_UNLOCK, &CUnLockFileTabDlg::OnBnClickedUnlock)
	ON_BN_CLICKED(BTN_SMASH, &CUnLockFileTabDlg::OnBnClickedSmash)
END_MESSAGE_MAP()


// CUnLockFileTabDlg 消息处理程序


void CUnLockFileTabDlg::OnBnClickedUnlock()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	DWORD dwRetBytes = 0;
	m_editPath.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		AfxMessageBox(TEXT("路径为空！"));
		return;
	}
	DiskSymbol2MSDos(strPath);
	if (!DeviceIoControl(g_hDevice, FILE_UNLOCK, 
		strPath.GetBuffer(), strPath.GetLength() * sizeof(TCHAR) + sizeof(TCHAR), 
		NULL, NULL, &dwRetBytes, NULL))
	{
		AfxMessageBox(TEXT("文件解锁失败！"));
	}
	else
	{
		AfxMessageBox(TEXT("文件解锁成功！"));
	}
}


void CUnLockFileTabDlg::OnBnClickedSmash()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	DWORD dwRetBytes = 0;
	m_editPath.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		AfxMessageBox(TEXT("路径为空！"));
		return;
	}
	DiskSymbol2MSDos(strPath);
	if (AfxMessageBox(L"确定要粉碎文件吗？", MB_OKCANCEL) == IDOK)
	{
		DiskSymbol2MSDos(strPath);
		if (!DeviceIoControl(g_hDevice, FILE_SMASH,
			strPath.GetBuffer(), strPath.GetLength() * sizeof(TCHAR) + sizeof(TCHAR),
			NULL, NULL, &dwRetBytes, NULL))
		{
			AfxMessageBox(TEXT("文件粉碎失败！"));
		}
		else
		{
			AfxMessageBox(TEXT("文件粉碎成功！"));
		}
	}
}



void CUnLockFileTabDlg::DiskSymbol2MSDos(CString& strPath)
{
	WCHAR ch = L'C';
	WCHAR szDisk[] = L"C:";
	WCHAR szBuf[MAX_PATH] = { 0 };
	for (; ch <= L'Z'; ch++)
	{
		szDisk[0] = ch;
		if (strPath.Find(szDisk) == -1)
		{
			continue;
		}
		if (QueryDosDevice(szDisk, szBuf, MAX_PATH))
		{
			if (strPath.Replace(szDisk, szBuf))
			{
				break;
			}
		}
	}
}

BOOL CUnLockFileTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		CRect tabRect;
		CRect editRect;
		CRect btnRect;
		CRect textRect;
		m_editPath.GetClientRect(&editRect);
		m_btnSmash.GetClientRect(&btnRect);
		m_textPath.GetClientRect(&textRect);
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);

		m_editPath.MoveWindow(tabRect.Width() / 2 - editRect.Width() / 2, 
			tabRect.Height() / 2 - 50,
			tabRect.Width()/2,
			textRect.Height());

		m_btnUnlock.MoveWindow(tabRect.Width() / 2 - 70 - btnRect.Width(),
			tabRect.Height() / 2 + 50,
			btnRect.Width(),
			btnRect.Height());

		m_btnSmash.MoveWindow(tabRect.Width() / 2 + 70,
			tabRect.Height() / 2 + 50,
			btnRect.Width(),
			btnRect.Height());


		/*CString fmt;
		fmt.Format(L"%d, %d, %d, %d", editRect.left - textRect.Width(), editRect.top, textRect.Width(), textRect.Height());
		AfxMessageBox(fmt);*/
		m_textPath.MoveWindow(tabRect.Width() / 2 - editRect.Width() / 2 - textRect.Width(),
			tabRect.Height() / 2 - 50,
			textRect.Width(),
			textRect.Height());


	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
