#pragma once

#include "CThreadChildDlg.h"
#include "CMemoryChildDlg.h"

// CProcessTabDlg 对话框

class CProcessTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessTabDlg)

public:
	CProcessTabDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuThread();
	afx_msg void OnMenuProcessExit();
	afx_msg void OnMenuProcessMem();
	afx_msg void OnRclickListProc(NMHDR* pNMHDR, LRESULT* pResult);

public:
	void InsertListItem();
	void MSDos2DiskSymbol(CString& strPath);		//符号链接名转盘符

public:
	CListCtrl m_lcProcess;
	DWORD m_dwCurIdx;
};
