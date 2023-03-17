#pragma once


// CSSDTTabDLg 对话框

class CSSDTTabDLg : public CDialogEx
{
	DECLARE_DYNAMIC(CSSDTTabDLg)

public:
	CSSDTTabDLg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSSDTTabDLg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_DLG_SSDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	void InsertListItem();
	PVOID GetOriginalSSDTFunction(DWORD dwIdx, BOOL bIsGdi);
	CListCtrl m_lcSSDT;
	DWORD m_dwNtRVA;
	DWORD m_dwGdiRVA;
};
