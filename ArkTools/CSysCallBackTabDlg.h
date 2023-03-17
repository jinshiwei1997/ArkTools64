#pragma once


// CSysCallBackTabDlg 对话框

class CSysCallBackTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSysCallBackTabDlg)

public:
	CSysCallBackTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSysCallBackTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_SYSCALLBACK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InsertListItem();

	CListCtrl m_lcCallback;
	PVOID m_pPspCreateProcessNotifyRoutine;
	PVOID m_pPspCreateThreadNotifyRoutine;
};
