#pragma once

#include "dia2.h"
#include "cvconst.h"
#include "diacreate.h"
#include <string>
#pragma comment(lib, "diaguids.lib")

class CDumpPdb
{

public:
    CDumpPdb();
    CDumpPdb(const wchar_t* szFilename);
    ~CDumpPdb();

    unsigned int SymToRva(wchar_t* wszSymName);
    std::wstring RvaToSym(int nRVA);
    void Init(const wchar_t* szFilename);
    BOOL IsInit();
private:
    bool LoadDataFromPdb(const wchar_t* szFilename, IDiaDataSource** ppSource, IDiaSession** ppSession, IDiaSymbol** ppGlobal);
    BOOL m_bInit = FALSE;
    IDiaDataSource* m_pDiaDataSource;
    IDiaSession* m_pDiaSession;
    IDiaSymbol* m_pGlobalSymbol;
    DWORD m_dwMachineType = CV_CFL_80386;
};

