#include "CDumpPdb.h"
#include "string"
using namespace std;

CDumpPdb::CDumpPdb()
{
}

CDumpPdb::CDumpPdb(const wchar_t* szFilename)
{
    Init(szFilename);
}

CDumpPdb::~CDumpPdb()
{
    if (m_pGlobalSymbol)
    {
        m_pGlobalSymbol->Release();
        m_pGlobalSymbol = NULL;
    }

    if (m_pDiaSession)
    {
        m_pDiaSession->Release();
        m_pDiaSession = NULL;
    }
    if (m_pDiaDataSource)
    {
        m_pDiaDataSource->Release();
        m_pDiaSession = NULL;
    }
    CoUninitialize();
}

bool CDumpPdb::LoadDataFromPdb(const wchar_t* szFilename,
    IDiaDataSource** ppSource,
    IDiaSession** ppSession,
    IDiaSymbol** ppGlobal)
{
    
    wchar_t wszExt[MAX_PATH];
    wchar_t wszErr[MAXBYTE] = { 0 };
    DWORD dwMachType = 0;

    HRESULT hr = CoInitialize(NULL);

    hr = NoRegCoCreate(L"msdia140.dll", // 直接使用dll名
        __uuidof(DiaSource),
        __uuidof(IDiaDataSource),
        (void**)ppSource);

    if (FAILED(hr)) 
    {
        wsprintfW(wszErr, L"CoCreateInstance failed - HRESULT = %08X\n", hr);
        OutputDebugString(wszErr);
        return false;
    }

    _wsplitpath_s(szFilename, NULL, 0, NULL, 0, NULL, 0, wszExt, MAX_PATH);

    if (!_wcsicmp(wszExt, L".pdb")) 
    {
        // Open and prepare a program database (.pdb) file as a debug data source
        hr = (*ppSource)->loadDataFromPdb(szFilename);
        if (FAILED(hr)) {
            wsprintfW(wszErr, L"loadDataFromPdb failed - HRESULT = %08X\n", hr);
            OutputDebugString(wszErr);
            return false;
        }
    }

    // Open a session for querying symbols
    hr = (*ppSource)->openSession(ppSession);
    if (FAILED(hr)) 
    {
        wsprintfW(wszErr, L"openSession failed - HRESULT = %08X\n", hr);
        OutputDebugString(wszErr);
        return false;
    }

    // Retrieve a reference to the global scope
    hr = (*ppSession)->get_globalScope(ppGlobal);
    if (hr != S_OK) 
    {
        wsprintfW(wszErr, L"get_globalScope failed\n");
        OutputDebugString(wszErr);
        return false;
    }

    // Set Machine type for getting correct register names
    if ((*ppGlobal)->get_machineType(&dwMachType) == S_OK) {
        switch (dwMachType) {
        case IMAGE_FILE_MACHINE_I386: m_dwMachineType = CV_CFL_80386; break;
        case IMAGE_FILE_MACHINE_IA64: m_dwMachineType = CV_CFL_IA64; break;
        case IMAGE_FILE_MACHINE_AMD64: m_dwMachineType = CV_CFL_AMD64; break;
        }
    }

    return true;
}

unsigned int CDumpPdb::SymToRva(wchar_t* wszSymName)
{
    IDiaEnumSymbols* pEnumSymbols;

    if (FAILED(m_pGlobalSymbol->findChildren(SymTagPublicSymbol, NULL, nsNone, &pEnumSymbols))) {
        return -1;
    }

    IDiaSymbol* pSymbol;
    ULONG celt = 0;
    DWORD dwRVA = -1;
    wstring ws2 = wszSymName;
    for (size_t i = 0; ws2[i] != '\0'; i++)
    {
        ws2[i] = towupper(ws2[i]);
    }

    while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1)) {
        BSTR str;
        wstring ws1;

        pSymbol->get_name(&str);
        ws1 = str;
        SysFreeString(str);

        for (size_t i = 0; ws1[i] != '\0'; i++)
        {
            ws1[i] = towupper(ws1[i]);
        }
        int nIdx = ws1.find(ws2);
        if (nIdx != -1)
        {
            WCHAR wch = ws1[nIdx + ws2.length()];
            if (wch > L'A' && wch < L'Z')
            {

            }
            else
            {
                if (pSymbol->get_relativeVirtualAddress(&dwRVA) == S_OK)
                {
                    pSymbol->Release();
                    break;
                }
                else
                {
                    dwRVA = -1;
                }
            }
        }
        pSymbol->Release();
    }
    pEnumSymbols->Release();
    return dwRVA;
}

std::wstring CDumpPdb::RvaToSym(int nRVA)
{
    wstring wstrRet;
    BSTR Name;
    IDiaSymbol* pSymbol = NULL;
    LONG lDisplacement;
    m_pDiaSession->findSymbolByRVAEx(nRVA, SymTagNull, &pSymbol, &lDisplacement);
    pSymbol->get_name(&Name);
    wstrRet = Name;
    pSymbol->Release();
    return wstrRet;
}

void CDumpPdb::Init(const wchar_t* szFilename)
{
    m_bInit = LoadDataFromPdb(szFilename, &m_pDiaDataSource, &m_pDiaSession, &m_pGlobalSymbol);
}

BOOL CDumpPdb::IsInit()
{
    return m_bInit;
}

