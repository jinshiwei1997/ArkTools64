
// ArkToolsDlg.h: 头文件
//

#pragma once

#include "global.h"
#include "CProcessTabDlg.h"
#include "CFileTabDlg.h"
#include "CRegTabDlg.h"
#include "CGDTDlg.h"
#include "CDriverTabDlg.h"
#include "CSSDTTabDLg.h"
#include "CUnLockFileTabDlg.h"
#include "CSysCallBackTabDlg.h"
#include "CIDTDlg.h"

// CArkToolsDlg 对话框
class CArkToolsDlg : public CDialogEx
{
// 构造
public:
	CArkToolsDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARKTOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void Init();
	void InitTabCtrl();
	BOOL InitPDB();
	BOOL LoadDriver();
	BOOL UnLoadDriver();
	BOOL InitKrnlFile();
	PVOID LoadPE(wchar_t* szPath, DWORD dwPathSize, DWORD dwRelocationBase, CList<SECTION, SECTION&>* pCheckList, PVOID* pImgBase);

public:
	enum TabType
	{
		TT_PROCESS,
		TT_DRIVER,
		TT_FILE,
		TT_REGEDIT,
		//TT_SERVICE,
		//TT_MSCONFIG,
		TT_GDT,
		TT_IDT,
		TT_SSDT,
		//TT_HOOK,
		TT_FILEUNLOCK,
		TT_SYSCALLBACK,
		TT_NONE
	};

	SC_HANDLE			m_hSCManager = NULL;
	CTabCtrl			m_TabCtrl;
	CProcessTabDlg		m_TabDlgProcess;
	CDriverTabDlg		m_TabDlgDriver;
	CFileTabDlg			m_TabDlgFile;
	CRegTabDlg			m_TabDlgReg;
	CGDTDlg				m_TabDlgGDT;
	CIDTDlg				m_TabDlgIDT;
	CSSDTTabDLg			m_TabDlgSSDT;
	CUnLockFileTabDlg	m_TabDlgUnLockFile;
	CSysCallBackTabDlg	m_TabDlgSysCallback;
};
