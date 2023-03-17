// CRegTabDlg.cpp: 实现文件
//

#include "pch.h"
#include "ArkTools.h"
#include "CRegTabDlg.h"
#include "afxdialogex.h"
#include "global.h"

// CRegTabDlg 对话框

IMPLEMENT_DYNAMIC(CRegTabDlg, CDialogEx)

CRegTabDlg::CRegTabDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_DLG_REG, pParent)
{

}

CRegTabDlg::~CRegTabDlg()
{
}

void CRegTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_REG, m_lcReg);
	DDX_Control(pDX, TREE_REG, m_tcReg);
}


BEGIN_MESSAGE_MAP(CRegTabDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, TREE_REG, &CRegTabDlg::OnSelchangedTreeReg)
	ON_NOTIFY(TVN_ITEMEXPANDING, TREE_REG, &CRegTabDlg::OnItemexpandingTreeReg)
END_MESSAGE_MAP()


// CRegTabDlg 消息处理程序

BOOL CRegTabDlg::OnInitDialog()
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
			CRect lcRect = tabRect;
			lcRect.DeflateRect(tabRect.Width() / 4 + 5, 0, 0, 0);
			m_lcReg.MoveWindow(lcRect);

			/*标题*/
			DWORD i = 0;
			m_lcReg.InsertColumn(i++, L"名称");
			m_lcReg.InsertColumn(i++, L"类型");
			m_lcReg.InsertColumn(i++, L"数据");

			/*设置列表每列标题宽度*/
			i = 0;
			DWORD dwWidth = lcRect.Width() / 4;
			m_lcReg.SetColumnWidth(i++, dwWidth);
			m_lcReg.SetColumnWidth(i++, dwWidth);
			m_lcReg.SetColumnWidth(i++, lcRect.Width() - dwWidth * 2);

			/*设置列表风格*/
			m_lcReg.SetExtendedStyle(m_lcReg.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		}

		/*树控件*/
		{
			CRect tcRect = tabRect;
			tcRect.DeflateRect(0, 0, tabRect.Width() / 4 * 3 + 5, 0);
			m_tcReg.MoveWindow(tcRect);

			/*设置风格*/
			m_tcReg.ModifyStyle(0, TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES);

			/*插入初始项*/
			CString str[5] = { _T("HKEY_CLASSES_ROOT"),
				_T("HKEY_CURRENT_USER") ,
				_T("HKEY_LOCAL_MACHINE") ,
				_T("HKEY_USERS") ,
				_T("HKEY_CURRENT_CONFIG") };
			for (int i = 0; i < 5; i++)
			{
				HTREEITEM hTree = m_tcReg.InsertItem(str[i]);
				m_tcReg.InsertItem(NULL, hTree);
			}
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRegTabDlg::OnSelchangedTreeReg(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	m_lcReg.DeleteAllItems();

	HTREEITEM hCurTree = pNMTreeView->itemNew.hItem;
	CString strPath = GetPath(m_tcReg.GetParentItem(hCurTree), m_tcReg.GetItemText(hCurTree));
	InsertListItem(hCurTree, strPath);

	*pResult = 0;
}


void CRegTabDlg::OnItemexpandingTreeReg(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString strPath = GetPath(m_tcReg.GetParentItem(hItem), m_tcReg.GetItemText(hItem));
	OutputDebugString(strPath.GetBuffer());
	if (pNMTreeView->action == TVE_EXPAND)
	{
		m_tcReg.DeleteItem(m_tcReg.GetChildItem(hItem));
		InsertTreeNode(hItem, strPath);
	}
	else if (pNMTreeView->action == TVE_COLLAPSE)
	{
		HTREEITEM hChild = m_tcReg.GetChildItem(hItem);
		while (hChild != NULL)
		{
			//清空子节点
			HTREEITEM hNext = m_tcReg.GetNextItem(hChild, TVGN_NEXT);
			m_tcReg.DeleteItem(hChild);
			hChild = hNext;
		}
		//添加空节点激活+-按钮
		m_tcReg.InsertItem(NULL, hItem);
	}
	*pResult = 0;
}


/*============================================================================*/

CString CRegTabDlg::GetPath(HTREEITEM hParentItem, CString strPath)
{
	CString strTmp = m_tcReg.GetItemText(hParentItem);
	BOOL b = strTmp.IsEmpty();

	if (strPath == _T("HKEY_LOCAL_MACHINE") || strTmp == _T("HKEY_LOCAL_MACHINE"))
	{
		strTmp = _T("\\Registry\\Machine");
	}
	else if (strPath == _T("HKEY_USERS") || strTmp == _T("HKEY_USERS"))
	{
		strTmp = _T("\\Registry\\User");
	}
	else if (strPath == _T("HKEY_CLASSES_ROOT") || strTmp == _T("HKEY_CLASSES_ROOT"))
	{
		strTmp = _T("\\Registry\\Machine\\SOFTWARE\\Classes");
	}
	else if (strPath == _T("HKEY_CURRENT_CONFIG") || strTmp == _T("HKEY_CURRENT_CONFIG"))
	{
		strTmp = _T("\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Hardware Profiles\\Current");
	}
	else if (strPath == _T("HKEY_CURRENT_USER") || strTmp == _T("HKEY_CURRENT_USER"))
	{
		WCHAR pOutBuf[260] = { 0 };
		ULONG nBufSize = 520;
		ULONG nRetBytes = 0;
		DeviceIoControl(g_hDevice, CUR_USER_PATH, NULL, 0, pOutBuf, nBufSize, &nRetBytes, NULL);
		strTmp = pOutBuf;
		strTmp.Delete(0, strTmp.ReverseFind(_T('\\')));
		strTmp = CString(L"\\Registry\\User") + strTmp;
	}
	if (!b)
	{
		strTmp += _T("\\");
		strTmp += strPath;
	}

	HTREEITEM hParent = m_tcReg.GetParentItem(hParentItem);
	if (hParent == NULL)
	{
		return strTmp;
	}
	return GetPath(hParent, strTmp);
}

void CRegTabDlg::InsertTreeNode(HTREEITEM hItem, CString strPath)
{
	DWORD dwRetByte = 0;
	REG_SIZE_INFO fSize = { 0 };
	REG_KEY_INFO* pKeyInfo = nullptr;

	//获取缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_REGISTER_SIZE,
		strPath.GetBuffer(), strPath.GetLength() * 2 + 2,
		&fSize, sizeof(REG_SIZE_INFO),
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"获取注册表所需缓冲区大小失败!");
		return;
	}

	//获取子项
	pKeyInfo = (REG_KEY_INFO*)new char[fSize.m_nSubKeys * sizeof(REG_KEY_INFO)];
	if (pKeyInfo == nullptr)
	{
		AfxMessageBox(L"申请内存失败!");
		return;
	}
	if (!DeviceIoControl(g_hDevice, ENUM_REGISTER_KEY,
		strPath.GetBuffer(), strPath.GetLength() * 2 + 2,
		pKeyInfo, fSize.m_nSubKeys * sizeof(REG_KEY_INFO),
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"获取注册表子项失败!");
		delete[]pKeyInfo;
		return;
	}
	//遍历
	for (int i = 0; i < fSize.m_nSubKeys; i++)
	{
		HTREEITEM h = m_tcReg.InsertItem(pKeyInfo[i].m_szKeyName, hItem);
		if (!IsEmptyKey(CString(strPath) + L"\\" + pKeyInfo[i].m_szKeyName))
		{
			m_tcReg.InsertItem(NULL, h);
		}
	}

	delete[]pKeyInfo;
	return;
}

BOOL CRegTabDlg::IsEmptyKey(CString strPath)
{
	DWORD dwRetByte = 0;
	REG_SIZE_INFO fSize = { 0 };
	//获取缓冲区所需大小
	DeviceIoControl(g_hDevice, ENUM_REGISTER_SIZE,
		strPath.GetBuffer(), strPath.GetLength() * 2 + 2,
		&fSize, sizeof(REG_SIZE_INFO),
		&dwRetByte, NULL);
	if (fSize.m_nSubKeys > 0)
	{
		return FALSE;
	}

	return TRUE;
}

void CRegTabDlg::InsertListItem(HTREEITEM hItem, CString strPath)
{
	DWORD dwRetByte = 0;
	REG_SIZE_INFO RegSize = { 0 };
	REG_VAL_INFO* pValInfo = nullptr;

	//获取缓冲区所需大小
	if (!DeviceIoControl(g_hDevice, ENUM_REGISTER_SIZE,
		strPath.GetBuffer(), strPath.GetLength() * 2 + 2,
		&RegSize, sizeof(REG_SIZE_INFO),
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"获取注册表所需缓冲区大小失败!");
		return;
	}

	//获取子项
	pValInfo = (REG_VAL_INFO*)new char[RegSize.m_nValues * sizeof(REG_VAL_INFO)];
	if (pValInfo == nullptr)
	{
		AfxMessageBox(L"申请内存失败!");
		return;
	}
	if (!DeviceIoControl(g_hDevice, ENUM_REGISTER_VAL,
		strPath.GetBuffer(), strPath.GetLength() * 2 + 2,
		pValInfo, RegSize.m_nValues * sizeof(REG_VAL_INFO),
		&dwRetByte, NULL))
	{
		AfxMessageBox(L"获取注册表变量失败!");
		delete[]pValInfo;
		return;
	}
	//遍历
	CString fmt;
	for (int i = 0; i < RegSize.m_nValues; i++)
	{
		CString strName = pValInfo[i].m_szValName;
		if (strName.IsEmpty())
		{
			strName = L"(默认)";
		}
		m_lcReg.InsertItem(i, strName);
		switch (pValInfo[i].m_nValType)
		{
		case REG_NONE:
			m_lcReg.SetItemText(i, 1, L"REG_NONE");
			for (size_t j = 0; j < pValInfo[i].m_nDataLen; j++)
			{
				CString s;
				s.Format(L"%02X ", pValInfo[i].m_szValData[j]);
				fmt += s;
			}
			break;
		case REG_BINARY:
			m_lcReg.SetItemText(i, 1, L"REG_BINARY");
			for (size_t j = 0; j < pValInfo[i].m_nDataLen; j++)
			{
				CString s;
				s.Format(L"%02X ", pValInfo[i].m_szValData[j]);
				fmt += s;
			}
			break;
		case REG_DWORD:
			m_lcReg.SetItemText(i, 1, L"REG_DWORD");
			fmt.Format(L"0x%08X", *(DWORD*)pValInfo[i].m_szValData);
			break;
		case REG_DWORD_BIG_ENDIAN:
			m_lcReg.SetItemText(i, 1, L"REG_DWORD_BIG_ENDIAN");
			fmt.Format(L"0x%02X%02X%02X%02X", pValInfo[i].m_szValData[0], pValInfo[i].m_szValData[1], pValInfo[i].m_szValData[2], pValInfo[i].m_szValData[3]);
			break;
		case REG_EXPAND_SZ:
			m_lcReg.SetItemText(i, 1, L"REG_EXPAND_SZ");
			fmt.Format(L"%s", pValInfo[i].m_szValData);
			break;
		case REG_LINK:
			m_lcReg.SetItemText(i, 1, L"REG_LINK");
			fmt.Format(L"%s", pValInfo[i].m_szValData);
			break;
		case REG_MULTI_SZ:
			m_lcReg.SetItemText(i, 1, L"REG_MULTI_SZ");
			fmt.Format(L"%s", pValInfo[i].m_szValData);
			break;
		case REG_SZ:
			m_lcReg.SetItemText(i, 1, L"REG_SZ");
			fmt.Format(L"%s", pValInfo[i].m_szValData);
			break;
		}
		m_lcReg.SetItemText(i, 2, fmt);
	}

	delete[]pValInfo;
	return;
}



