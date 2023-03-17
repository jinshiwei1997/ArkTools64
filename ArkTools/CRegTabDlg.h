#pragma once


// CRegTabDlg 对话框

class CRegTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegTabDlg)

public:
	CRegTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRegTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_REG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString			GetPath(HTREEITEM hParentItem, CString strPath);
	void			InsertTreeNode(HTREEITEM hItem, CString strPath);
	BOOL			IsEmptyKey(CString strPath);
	void			InsertListItem(HTREEITEM hItem, CString strPath);

public:
	CListCtrl m_lcReg;
	CTreeCtrl m_tcReg;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeReg(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingTreeReg(NMHDR* pNMHDR, LRESULT* pResult);
};
