#pragma once
#include "global.h"

// CGDTDlg 对话框

class CGDTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGDTDlg)

public:
	CGDTDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGDTDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_GDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	void InsertListItem();
	CString GetTypeString(SEG_DESCRIPTOR& sd);
public:
	CListCtrl m_lcGDT;
};
