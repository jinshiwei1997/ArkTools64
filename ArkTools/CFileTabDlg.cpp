// CFileTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CFileTabDlg.h"
#include "afxdialogex.h"
#include "global.h"

// CFileTabDlg 对话框

IMPLEMENT_DYNAMIC(CFileTabDlg, CDialogEx)

CFileTabDlg::CFileTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_FILE, pParent)
{

}

CFileTabDlg::~CFileTabDlg()
{
}

void CFileTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_FILE, m_lcFile);
	DDX_Control(pDX, TREE_FILE, m_tcFile);
}


BEGIN_MESSAGE_MAP(CFileTabDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, TREE_FILE, &CFileTabDlg::OnSelchangedTreeFile)
	ON_NOTIFY(TVN_ITEMEXPANDING, TREE_FILE, &CFileTabDlg::OnItemexpandingTreeFile)
END_MESSAGE_MAP()


// CFileTabDlg 消息处理程序


BOOL CFileTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		CRect tabRect;
		pParent->GetClientRect(&tabRect);
		tabRect.DeflateRect(0, 0, 7, 35);

		/*列表控件*/
		{
			/*设置长宽以及风格*/
			CRect lcRect = tabRect;
			lcRect.DeflateRect(tabRect.Width() / 4 + 5, 0, 0, 0);
			m_lcFile.MoveWindow(lcRect);
			m_lcFile.SetExtendedStyle(m_lcFile.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			/*设置图标*/
			m_ListImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 8);
			m_lcFile.SetImageList(&m_ListImg, LVSIL_SMALL);

			/*标题*/
			DWORD i = 0;
			m_lcFile.InsertColumn(i++, L"文件名");
			m_lcFile.InsertColumn(i++, L"创建时间");
			m_lcFile.InsertColumn(i++, L"修改时间");
			m_lcFile.InsertColumn(i++, L"文件类型");
			m_lcFile.InsertColumn(i++, L"大小");

			/*设置列表每列标题宽度*/
			i = 0;
			DWORD dwWidth = lcRect.Width() / 6;
			m_lcFile.SetColumnWidth(i++, lcRect.Width() - dwWidth * 4);
			m_lcFile.SetColumnWidth(i++, dwWidth);
			m_lcFile.SetColumnWidth(i++, dwWidth);
			m_lcFile.SetColumnWidth(i++, dwWidth);
			m_lcFile.SetColumnWidth(i++, dwWidth);
		}

		/*树控件*/
		{
			/*设置长宽以及风格*/
			CRect tcRect = tabRect;
			tcRect.DeflateRect(0, 0, tabRect.Width() / 4 * 3 + 5, 0);
			m_tcFile.MoveWindow(tcRect);
			m_tcFile.ModifyStyle(0, TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES);

			/*获取盘符*/
			WCHAR szBuf[MAXBYTE] = { 0 };
			DWORD dwLength = GetLogicalDriveStrings(MAXBYTE, szBuf);

			/*获取盘符图标*/
			m_TreeImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 8);
			m_tcFile.SetImageList(&m_TreeImg, TVSIL_NORMAL);
			SHFILEINFO fi = { 0 };
			SHGetFileInfo(szBuf, NULL, &fi, sizeof(fi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME);
			m_TreeImg.Add(fi.hIcon);

			/*插入初始盘符节点*/
			for (DWORD i = 0; i < dwLength; i++)
			{
				if (i == 0 || *(szBuf + i - 1) == '\0')
				{
					TCHAR ch = *(szBuf + i);
					CString str(ch);
					str += ':';
					HTREEITEM hTree = m_tcFile.InsertItem(CString(str), 0, 0);
					m_tcFile.InsertItem(NULL, hTree);	//插入空节点激活+-按钮
				}
			}

		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CFileTabDlg::OnSelchangedTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_lcFile.DeleteAllItems();
	m_ListImg.DeleteImageList();
	m_ListImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 8);
	m_lcFile.SetImageList(&m_ListImg, LVSIL_SMALL);

	HTREEITEM hCurTree = pNMTreeView->itemNew.hItem;
	CString strPath = GetPath(m_tcFile.GetParentItem(hCurTree), m_tcFile.GetItemText(hCurTree));
	InsertNodeOrItem(hCurTree, strPath, IM_LIST);
	*pResult = 0;
}


void CFileTabDlg::OnItemexpandingTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString strPath = GetPath(m_tcFile.GetParentItem(hItem), m_tcFile.GetItemText(hItem));
	OutputDebugString(strPath.GetBuffer());
	if (pNMTreeView->action == TVE_EXPAND)
	{
		m_tcFile.DeleteItem(m_tcFile.GetChildItem(hItem));
		InsertNodeOrItem(hItem, strPath, IM_TREE);
	}
	else if (pNMTreeView->action == TVE_COLLAPSE)
	{
		HTREEITEM hChild = m_tcFile.GetChildItem(hItem);
		while (hChild != NULL)
		{
			//清空子节点
			HTREEITEM hNext = m_tcFile.GetNextItem(hChild, TVGN_NEXT);
			m_tcFile.DeleteItem(hChild);
			hChild = hNext;
		}
		//添加空节点激活+-按钮
		m_tcFile.InsertItem(NULL, hItem);
	}

	*pResult = 0;
}

void CFileTabDlg::InsertNodeOrItem(HTREEITEM hItem, CString strPath, InsertModel model)
{
	DWORD dwRetByte = 0;
	DWORD dwCurByte = 0;
	FILE_INFO* pFileInfo = nullptr;
	WCHAR* pBuf = new WCHAR[65535 * 2];
	if (pBuf == nullptr)
	{
		AfxMessageBox(L"内存申请失败！");
		return;
	}
	RtlZeroMemory(pBuf, 65535 * 2 * 2);
	pFileInfo = (FILE_INFO*)pBuf;

	DWORD dwSize = strPath.GetLength() * 2 + 2;
	if (!DeviceIoControl(g_hDevice, ENUM_FILE,
		strPath.GetBuffer(), dwSize,
		pBuf, 65535 * 2,
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"DeviceIoControl Error!");
		delete[]pBuf;
		return;
	}

	int i = 0;
	while (dwCurByte < dwRetByte)
	{
		CString r3Path = strPath;
		if (r3Path.Right(1) != "\\")
		{
			r3Path += "\\";
		}
		r3Path += pFileInfo->m_szName;
		r3Path = r3Path.GetBuffer() + 4;
		SHFILEINFO fi = { 0 };
		BOOL bRet = SHGetFileInfo(r3Path, NULL, &fi, sizeof(fi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME);
		if (!bRet)
		{
			CString fmt;
			fmt.Format(L"SHGetFileInfo err, path:%s", r3Path);
			OutputDebugString(fmt.GetBuffer());
		}
		if (pFileInfo->m_nFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (model & IM_TREE)
			{
				if (m_bTreeImgFlag == FALSE)
				{
					m_TreeImg.Add(fi.hIcon);
					m_bTreeImgFlag = TRUE;
				}
				if (IsEmptyDirectory(r3Path))
				{
					m_tcFile.InsertItem(pFileInfo->m_szName, 1, 1, hItem);
				}
				else
				{
					HTREEITEM h = m_tcFile.InsertItem(pFileInfo->m_szName, 1, 1, hItem);
					m_tcFile.InsertItem(NULL, h);
				}
			}
		}
		else
		{
			if (model & IM_LIST)
			{
				CString fmt;
				SYSTEMTIME CreateTime = { 0 };
				SYSTEMTIME ChangeTime = { 0 };
				FileTimeToSystemTime((FILETIME*)&pFileInfo->m_nCreateTime, &CreateTime);
				FileTimeToSystemTime((FILETIME*)&pFileInfo->m_nChangeTime, &ChangeTime);

				/*插入*/
				int j = 1;
				m_lcFile.InsertItem(i, pFileInfo->m_szName, m_ListImg.Add(fi.hIcon));
				fmt.Format(L"%04d-%02d-%02d %02d:%02d\r\n", CreateTime.wYear, CreateTime.wMonth, CreateTime.wDay, CreateTime.wHour, CreateTime.wMinute);
				m_lcFile.SetItemText(i, j++, fmt);
				fmt.Format(L"%04d-%02d-%02d %02d:%02d\r\n", ChangeTime.wYear, ChangeTime.wMonth, ChangeTime.wDay, ChangeTime.wHour, ChangeTime.wMinute);
				m_lcFile.SetItemText(i, j++, fmt);
				m_lcFile.SetItemText(i, j++, fi.szTypeName);
				fmt.Format(L"%llu", pFileInfo->m_nFileSize.QuadPart);
				m_lcFile.SetItemText(i, j++, fmt);
				i++;
			}
		}

		pFileInfo++;
		dwCurByte += sizeof(FILE_INFO);
	}

	delete[]pBuf;
}

CString CFileTabDlg::GetPath(HTREEITEM hParentItem, CString strPath)
{
	CString strTmp = m_tcFile.GetItemText(hParentItem);
	if (strPath.Right(1) == _T(":"))
	{
		strTmp.Format(L"\\??\\%s\\", strPath);
	}
	else
	{
		if (strTmp.Right(1) == _T(":"))
		{
			CString tmp = strTmp;
			strTmp.Format(L"\\??\\%s", tmp);
		}
		strTmp += _T("\\");
		strTmp += strPath;
	}
	HTREEITEM hParent = m_tcFile.GetParentItem(hParentItem);
	if (hParent == NULL)
	{
		return strTmp;
	}
	return GetPath(hParent, strTmp);
}

BOOL CFileTabDlg::IsEmptyDirectory(CString strPath)
{
	CFileFind finder;
	strPath += _T("\\*.*");
	BOOL bWorking = finder.FindFile(strPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			return false;
		}
	}
	finder.Close();
	return true;
}


