#pragma once


// CMemoryChildDlg 对话框

class CMemoryChildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMemoryChildDlg)

public:
	CMemoryChildDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMemoryChildDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = CHILD_DLG_MEMORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedReadMemory();

	void InsertListItem();
	void WriteData();

	CEdit m_editReadAddr;
	CEdit m_editWriteAddr;
	CEdit m_editWriteData;
	CComboBox m_cbWriteType;
	CListCtrl m_lcData;
	HANDLE m_hPid;
	afx_msg void OnBnClickedWriteMemory();
};
