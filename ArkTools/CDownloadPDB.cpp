// CDownloadPDB.cpp: 实现文件
//

#include "pch.h"
#include "CDownloadPDB.h"

#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

// CDownloadPDB

IMPLEMENT_DYNAMIC(CDownloadPDB, CWnd)

CDownloadPDB::CDownloadPDB()
{

}

CDownloadPDB::CDownloadPDB(CString strFilePath)
{
    //m_strFileName = strFileName;
    Init(strFilePath);
    return;
}

CDownloadPDB::~CDownloadPDB()
{

}

void CDownloadPDB::Init(CString strFilePath)
{
    m_strFilePath = strFilePath;

    if (!GetUrl())
    {
        return;
    }

    HANDLE h = CreateFile(m_strPdbSavePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
        m_bCompleteFlag = TRUE;
        return;
    }
    if (CreatePdb() < 0)
    {
        return;
    }
    m_bCompleteFlag = TRUE;
}

BOOL CDownloadPDB::GetUrl()
{
    CString fmt;
    SYMSRV_INDEX_INFOW sii = { 0 };
    sii.sizeofstruct = sizeof(sii);
    if (!SymSrvGetFileIndexInfoW(m_strFilePath, &sii, 0))
    {
        fmt.Format(L"[testR3] SymSrvGetFileIndexInfoW Err, FilePath=%s, ErrCode=%p\r\n", m_strFilePath, GetLastError());
        OutputDebugString(fmt);
        return FALSE;
    }
    m_strUrl.Format(L"%s/%s/%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X/%s",
        L"http://msdl.blackint3.com:88/download/symbols",
        sii.pdbfile,
        sii.guid.Data1,
        sii.guid.Data2,
        sii.guid.Data3,
        sii.guid.Data4[0],
        sii.guid.Data4[1],
        sii.guid.Data4[2],
        sii.guid.Data4[3],
        sii.guid.Data4[4],
        sii.guid.Data4[5],
        sii.guid.Data4[6],
        sii.guid.Data4[7],
        sii.age,
        sii.pdbfile);

    WCHAR szPath[260] = { 0 };
    GetModuleFileName(NULL, szPath, 260 * sizeof(WCHAR));
    m_strPdbSavePath = szPath;
    int n = m_strPdbSavePath.ReverseFind(L'\\');
    m_strPdbSavePath.Delete(n, m_strFilePath.GetLength());
    m_strPdbSavePath += L"\\";
    m_strPdbSavePath += sii.pdbfile;
    return TRUE;
}

BOOL CDownloadPDB::IsComplete()
{
    return m_bCompleteFlag;
}

int CDownloadPDB::CreatePdb()
{
    unsigned short nPort;       //用于保存目标HTTP服务端口  
    CString strServer, strObject;   //strServer用于保存服务器地址，strObject用于保存文件对象名称  
    DWORD dwServiceType, dwRet;      //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号  
    
    //解析URL，获取信息  
    if (!AfxParseURL(m_strUrl, dwServiceType, strServer, strObject, nPort))
    {
        //解析失败，该Url不正确  
        return -1;
    }

    //创建网络连接对象，HTTP连接对象指针和用于该连接的HttpFile文件对象指针，注意delete  
    CInternetSession intsess;
    CHttpFile* pHtFile = NULL;
    //建立网络连接  
    CHttpConnection* pHtCon = intsess.GetHttpConnection(strServer, nPort);
    if (pHtCon == NULL)
    {
        //建立网络连接失败  
        intsess.Close();
        return -2;
    }
    //发起GET请求  
    pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
    if (pHtFile == NULL)
    {
        //发起GET请求失败  
        intsess.Close();
        delete pHtCon; pHtCon = NULL;
        return -3;
    }
    //提交请求  
    pHtFile->SendRequest();
    //获取服务器返回的状态号  
    pHtFile->QueryInfoStatusCode(dwRet);
    if (dwRet != HTTP_STATUS_OK)
    {
        //服务器不接受请求  
        intsess.Close();
        delete pHtCon; pHtCon = NULL;
        delete pHtFile; pHtFile = NULL;
        return -4;
    }
    //获取文件大小  
    UINT nFileLen = (UINT)pHtFile->GetLength();

    DWORD dwRead = 1;       //用于标识读了多少，为1是为了进入循环  
    //创建缓冲区  
    CHAR* szBuffer = new CHAR[nFileLen + 1];

    TRY
    {
        CFile PicFile(m_strPdbSavePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
        //创建文件  
        while (dwRead > 0)
        {
            //清空缓冲区  
            memset(szBuffer,0,(size_t)(nFileLen + 1));
            //读取到缓冲区  
            dwRead = pHtFile->Read(szBuffer,nFileLen);
            //写入到文件  
            PicFile.Write(szBuffer,dwRead);
        }
        //关闭文件  
        PicFile.Close();
        //释放内存  
        delete[]szBuffer;
        delete pHtFile;
        delete pHtCon;
        //关闭网络连接  
        intsess.Close();
    }
        CATCH(CFileException, e)
    {
        //释放内存  
        delete[]szBuffer;
        delete pHtFile;
        delete pHtCon;
        //关闭网络连接  
        intsess.Close();
        return -7;          //读写文件异常  
    }
    END_CATCH
        return 1;
    return 0;
}

CString CDownloadPDB::GetPdbPath()
{
    return m_strPdbSavePath;
}


BEGIN_MESSAGE_MAP(CDownloadPDB, CWnd)
END_MESSAGE_MAP()



// CDownloadPDB 消息处理程序


