#pragma once

#include "dbghelp.h";
#include <afxinet.h>

// CDownloadPDB

class CDownloadPDB : public CWnd
{
	DECLARE_DYNAMIC(CDownloadPDB)

public:
	CDownloadPDB();
	CDownloadPDB(CString strFilePath);
	virtual ~CDownloadPDB();

protected:
	DECLARE_MESSAGE_MAP()

private:
	//CString m_strFileName;
	CString m_strFilePath;
	CString m_strPdbSavePath;
	CString m_strUrl;
	BOOL GetUrl();
	int CreatePdb();
	BOOL m_bCompleteFlag = FALSE;
public:
	void Init(CString strFilePath);
	BOOL IsComplete();
	CString GetPdbPath();
};


