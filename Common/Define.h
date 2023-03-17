#pragma once

#ifdef _WIN64
#define ADDR_LENGTH 16
#else
#define ADDR_LENGTH 8
#endif

#define DEVICE_NAME L"\\Device\\ArkDriver"
#define SYMBOL_NAME L"\\DosDevices\\ArkDriver"

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define ENUM_PROCESS            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_FILE               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_REGISTER_SIZE      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_REGISTER_KEY       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_REGISTER_VAL       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CUR_USER_PATH           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DELETE_FILE             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DELETE_REG_KEY          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DELETE_REG_VAL          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_DRIVER             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_GDT                CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_CR3                 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_PAGE_101012        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_PAGE_29912         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_PAGE_29912_PDPTE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ISPAE                   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define READ_PROC_MEMORY        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define WRITE_PROC_MEMORY       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_MEMORY             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define READ_SYS_MEMORY         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define READ_FILE               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FILE_UNLOCK             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FILE_SMASH              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_THREAD             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TERMINATE_PROCESS       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ENUM_SYSCALLBACK        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x825, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TEST                    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct tagSection           //3环用结构体
{
    UCHAR m_Name[9];
    ULONG m_dwRVA;
    ULONG m_dwSize;
}SECTION;

typedef struct tagProcessInfo
{
    HANDLE m_nPid;
    HANDLE m_nInheritedPid;
    PVOID m_pEPROCESS;
    WCHAR m_szName[260];
    WCHAR m_szPatch[260];
}PROCESS_INFO;

typedef struct tagFileInfo
{
    ULONG m_nFileAttributes;
    LARGE_INTEGER m_nCreateTime;
    LARGE_INTEGER m_nChangeTime;
    LARGE_INTEGER m_nFileSize;
    WCHAR m_szName[260];
}FILE_INFO;

typedef struct tagRegisterSizeInfo
{
    ULONG   m_nSubKeys;
    ULONG   m_nValues;
}REG_SIZE_INFO;

typedef struct tagRegisterKeyInfo
{
    ULONG   m_nSubKeys;
    ULONG   m_nNameLen;
    WCHAR   m_szKeyName[260];
}REG_KEY_INFO;

typedef struct tagRegisterValInfo
{
    ULONG   m_nValType;
    ULONG   m_nNameLen;
    WCHAR   m_szValName[260];
    ULONG   m_nDataLen;
    UCHAR   m_szValData[4096];
}REG_VAL_INFO;

typedef struct tagRegisterValDelete
{
    WCHAR   m_szRegPath[260];
    WCHAR   m_szValName[260];
}REG_VAL_DELETE;

typedef struct tagDriverInfo
{
    PVOID m_nBase;
    ULONG m_nSize;
    ULONG m_pObject;
    WCHAR m_szName[260];
    WCHAR m_szPatch[260];
}DRIVER_INFO;

typedef struct tagThreadInfo
{
    HANDLE m_uCid;
    ULONG m_uPriority;
    ULONG m_uState;
    ULONG m_uSwapCnt;
    PVOID m_pETHREAD;
    PVOID m_TEB;
    PVOID m_pStartAddr;
}THREAD_INFO;

typedef struct tagExitProcess
{
    HANDLE m_hPid;
    PVOID m_pfnPspTerminateThreadByPointer;
}PROCESS_EXIT;

typedef struct tagSystemCallback
{
    ULONG m_uCount;
    PVOID m_pNotifyRoutine;
}SYSTEM_CALLBACK;

#pragma warning(disable:4201)
#pragma pack(1)
typedef struct tagSegmentDescriptor
{
    union
    {
        struct 
        {
            ULONG Limit0 : 16;
            ULONG Base0 : 16;
            ULONG Base1 : 8;
            ULONG Type : 4;
            ULONG S : 1;
            ULONG DPL : 2;
            ULONG P : 1;
            ULONG Limit1 : 4;
            ULONG AVL : 1;
            ULONG Reserve : 1;
            ULONG DB : 1;
            ULONG G : 1;
            ULONG Base2 : 8;
        };
        struct 
        {
            ULONG uLow;
            ULONG uHigh;
        };
        ULONGLONG ull;
    };
}SEG_DESCRIPTOR;
#pragma pack()
#pragma pack(1)
typedef struct tagGateDescriptor
{
    ULONG Offset0 : 16;
    ULONG Selector : 16;
    ULONG IST : 3;
    ULONG : 5;
    ULONG Type : 4;
    ULONG S : 1;
    ULONG DPL : 2;
    ULONG P : 1;
    ULONG Offset1 : 16;
#ifdef _WIN64
    ULONG Offset2 : 32;
    ULONG : 32;
#endif // _WIN64
}GATE_DESCRIPTOR;
#pragma pack()

#pragma pack(1)
typedef struct tagGdt
{
    USHORT Size;
    PVOID SegDescriptor;
}GDT;

typedef struct tagPageAttribute
{
    union 
    {
        struct
        {
            USHORT P : 1;
            USHORT RW : 1;
            USHORT US : 1;
            USHORT PWT : 1;
            USHORT PCD : 1;
            USHORT A : 1;
            USHORT Reserve : 1;
            USHORT PS : 1;
            USHORT G : 1;
            USHORT Lgnored : 3;
            USHORT : 4;
        }PDE;
        struct
        {
            USHORT P : 1;
            USHORT RW : 1;
            USHORT US : 1;
            USHORT PWT : 1;
            USHORT PCD : 1;
            USHORT A : 1;
            USHORT D : 1;
            USHORT PAT : 1;
            USHORT G : 1;
            USHORT Lgnored : 3;
            USHORT : 4;
        }PTE;
        USHORT us;
    };
}PAGE_ATTRIBUTE;

typedef struct tagLinearAddress
{
    union 
    {
        struct 
        {
            ULONG Offset : 12;
            ULONG Table : 9;
            ULONG Directory : 9;
            ULONG DirectoryPointer : 2;

        };
        ULONG uLinearAddress;
    };
}LINEAR_ADDR;

typedef struct tagTargetProcInfo
{
    HANDLE m_hHandle;
    PVOID m_TargetAddres;
    unsigned __int64 m_nData;
}TARGET_PROC;

typedef struct tagFlag1
{
    union 
    {
        struct
        {
            ULONG CommitCharge : 19;
            ULONG NoChange : 1;
            ULONG VadType : 3;
            ULONG MemCommit : 1;
            ULONG Protection : 5;
            ULONG Spare : 2;
            ULONG PrivateMemory : 1;
        };
        ULONG u;
    };
}FLAG1;

typedef struct tagMemoryInfo
{
    ULONG m_NodeAddr;
    ULONG m_Level;
    ULONG m_StartAddr;
    ULONG m_EndAddr;
    FLAG1 m_Flag1;
    ULONG m_Flag2;
    UCHAR m_FilePath[260];
}MEM_INFO;


typedef struct _KSERVICE_TABLE_DESCRIPTOR
{
    PULONG_PTR Base;	//SSDT表的地址
    PULONG Count;		//计数
    SIZE_T Limit;		//SSDT表中函数个数
    PUCHAR Number;		//指向一个BYTE数组, 用来指出对应函数的参数大小
} KSERVICE_TABLE_DESCRIPTOR, * PKSERVICE_TABLE_DESCRIPTOR;


#pragma pack()
#pragma warning(default:4201)
