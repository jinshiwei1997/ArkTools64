#pragma once

// CThreadChildDlg 对话框

class CThreadChildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThreadChildDlg)

public:
	CThreadChildDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CThreadChildDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = CHILD_DLG_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InsertItem();
	CString StateToStr(DWORD state);

public:
	CListCtrl m_lcThread;
	HANDLE m_hPid;

};
