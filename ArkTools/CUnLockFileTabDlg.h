#pragma once


// CUnLockFileTabDlg 对话框

class CUnLockFileTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUnLockFileTabDlg)

public:
	CUnLockFileTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUnLockFileTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_UNLOCK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMFCEditBrowseCtrl m_editPath;
	afx_msg void OnBnClickedUnlock();
	afx_msg void OnBnClickedSmash();
	void DiskSymbol2MSDos(CString& strPath);
	virtual BOOL OnInitDialog();
	CButton m_btnUnlock;
	CButton m_btnSmash;
	CStatic m_textPath;
};
