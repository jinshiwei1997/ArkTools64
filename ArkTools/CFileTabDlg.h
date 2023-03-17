#pragma once


// CFileTabDlg 对话框

class CFileTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTabDlg)

public:
	CFileTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_FILE };
#endif
public:
	enum InsertModel
	{
		IM_TREE = 1,
		IM_LIST,
		IM_ALL
	};
	CListCtrl		m_lcFile;
	CTreeCtrl		m_tcFile;
	CImageList		m_TreeImg;
	CImageList		m_ListImg;
	BOOL			m_bTreeImgFlag = FALSE;
	InsertModel		m_model = IM_TREE;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingTreeFile(NMHDR* pNMHDR, LRESULT* pResult);

	void	InsertNodeOrItem(HTREEITEM hItem, CString strPath, InsertModel model);
	CString GetPath(HTREEITEM hParentItem, CString strPath);
	BOOL	IsEmptyDirectory(CString strPath);

};
