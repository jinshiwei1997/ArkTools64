#pragma once


// CIDTDlg 对话框

class CIDTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIDTDlg)

public:
	CIDTDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CIDTDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_IDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lcIDT;
	virtual BOOL OnInitDialog();
	void InsertListItem();
};
