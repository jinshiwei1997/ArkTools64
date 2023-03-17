#pragma once

// CDriverTabDlg 对话框

class CDriverTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverTabDlg)

public:
	CDriverTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_DRIVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	void InsertListItem();
	CListCtrl m_lcDriver;
};
