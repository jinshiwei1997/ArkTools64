#pragma once

//#include <ntddk.h>
#include <ntifs.h>
#include <intrin.h>
#include "..\Common\Define.h"
/*=============================导入函数声明=============================*/
#define MAX_PATH 260
#if defined (_WIN64)
#define MAX_FAST_REFS 15
#else
#define MAX_FAST_REFS 7
#endif
NTKERNELAPI CHAR* PsGetProcessImageFileName(PEPROCESS proc);
#pragma warning(disable:4201)
typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation,        //  0
    SystemProcessorInformation,        //  1
    SystemPerformanceInformation,        //  2
    SystemTimeOfDayInformation,        //  3
    SystemPathInformation,        //  4
    SystemProcessInformation,               //5
    SystemCallCountInformation,        //  6
    SystemDeviceInformation,        //  7
    SystemProcessorPerformanceInformation,        //  8
    SystemFlagsInformation,        //  9
    SystemCallTimeInformation,        //  10
    SystemModuleInformation,        //  11
    SystemLocksInformation,        //  12
    SystemStackTraceInformation,        //  13
    SystemPagedPoolInformation,        //  14
    SystemNonPagedPoolInformation,        //  15
    SystemHandleInformation,        //  16
    SystemObjectInformation,        //  17
    SystemPageFileInformation,        //  18
    SystemVdmInstemulInformation,        //  19
    SystemVdmBopInformation,        //  20
    SystemFileCacheInformation,        //  21
    SystemPoolTagInformation,        //  22
    SystemInterruptInformation,        //  23
    SystemDpcBehaviorInformation,        //  24
    SystemFullMemoryInformation,        //  25
    SystemLoadGdiDriverInformation,        //  26
    SystemUnloadGdiDriverInformation,        //  27
    SystemTimeAdjustmentInformation,        //  28
    SystemSummaryMemoryInformation,        //  29
    SystemMirrorMemoryInformation,        //  30
    SystemPerformanceTraceInformation,        //  31
    SystemObsolete0,        //  32
    SystemExceptionInformation,        //  33
    SystemCrashDumpStateInformation,        //  34
    SystemKernelDebuggerInformation,        //  35
    SystemContextSwitchInformation,        //  36
    SystemRegistryQuotaInformation,        //  37
    SystemExtendServiceTableInformation,        //  38
    SystemPrioritySeperation,        //  39
    SystemVerifierAddDriverInformation,        //  40
    SystemVerifierRemoveDriverInformation,        //  41
    SystemProcessorIdleInformation,        //  42
    SystemLegacyDriverInformation,        //  43
    SystemCurrentTimeZoneInformation,        //  44
    SystemLookasideInformation,        //  45
    SystemTimeSlipNotification,        //  46
    SystemSessionCreate,        //  47
    SystemSessionDetach,        //  48
    SystemSessionInformation,        //  49
    SystemRangeStartInformation,        //  50
    SystemVerifierInformation,        //  51
    SystemVerifierThunkExtend,        //  52
    SystemSessionProcessInformation,        //  53
    SystemLoadGdiDriverInSystemSpace,        //  54
    SystemNumaProcessorMap,        //  55
    SystemPrefetcherInformation,        //  56
    SystemExtendedProcessInformation,        //  57
    SystemRecommendedSharedDataAlignment,        //  58
    SystemComPlusPackage,        //  59
    SystemNumaAvailableMemory,        //  60
    SystemProcessorPowerInformation,        //  61
    SystemEmulationBasicInformation,        //  62
    SystemEmulationProcessorInformation,        //  63
    SystemExtendedHandleInformation,        //  64
    SystemLostDelayedWriteInformation,        //  65
    SystemBigPoolInformation,        //  66
    SystemSessionPoolTagInformation,        //  67
    SystemSessionMappedViewInformation,        //  68
    SystemHotpatchInformation,        //  69
    SystemObjectSecurityMode,        //  70
    SystemWatchdogTimerHandler,        //  71
    SystemWatchdogTimerInformation,        //  72
    SystemLogicalProcessorInformation,        //  73
    SystemWow64SharedInformation,        //  74
    SystemRegisterFirmwareTableInformationHandler,        //  75
    SystemFirmwareTableInformation,        //  76
    SystemModuleInformationEx,        //  77
    SystemVerifierTriageInformation,        //  78
    SystemSuperfetchInformation,        //  79
    SystemMemoryListInformation,        //  80
    SystemFileCacheInformationEx,        //  81
    MaxSystemInfoClass                      //82

} SYSTEM_INFORMATION_CLASS;



//处理进程信息，需要用到这两个结构体
typedef struct _SYSTEM_THREADS
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    ULONG WaitReason;
}SYSTEM_THREADS, * PSYSTEM_THREADS;

//进程信息结构体  
typedef struct _SYSTEM_PROCESSES
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER SpareLi1;
    LARGE_INTEGER SpareLi2;
    LARGE_INTEGER SpareLi3;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR PageDirectoryBase;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    struct _SYSTEM_THREADS          Threads[1];
}SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
    PVOID Reserved[2];
    PVOID Base;
    ULONG Size;
    ULONG Flags;
    USHORT Index;
    USHORT Unknown;
    USHORT LoadCount;
    USHORT ModuleNameOffset;
    CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, * PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
    ULONG Count;//内核中以加载的模块的个数
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;		//进程PID
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;		//句柄类型
    UCHAR HandleAttributes;		//安全属性
    USHORT HandleValue;			//句柄值
    PVOID Object;					//句柄对象
    ULONG GrantedAccess;			//该句柄的访问权限
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;		//句柄数量
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];	//句柄信息
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    KPRIORITY Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION;

typedef struct _EX_FAST_REF {
    union {
        PVOID Object;
#if defined (_WIN64)
        ULONG_PTR RefCnt : 4;
#else
        ULONG_PTR RefCnt : 3;
#endif
        ULONG_PTR Value;
    };
} EX_FAST_REF, * PEX_FAST_REF;

typedef struct _EX_CALLBACK {
    EX_FAST_REF RoutineBlock;
} EX_CALLBACK, * PEX_CALLBACK;

typedef struct _EX_CALLBACK_ROUTINE_BLOCK {
    EX_RUNDOWN_REF        RundownProtect;
    PEX_CALLBACK_FUNCTION Function;
    PVOID                 Context;
} EX_CALLBACK_ROUTINE_BLOCK, * PEX_CALLBACK_ROUTINE_BLOCK;

NTKERNELAPI
PVOID
PsGetThreadTeb(
    __in PETHREAD Thread
);

NTSTATUS ZwQuerySystemInformation(
  _In_      size_t                    SystemInformationClass,
  _Inout_   PVOID                    SystemInformation,
  _In_      ULONG                    SystemInformationLength,
  _Out_opt_ PULONG                   ReturnLength
);

NTSTATUS
NTAPI
ZwQueryInformationProcess(
    __in HANDLE ProcessHandle,
    __in PROCESSINFOCLASS ProcessInformationClass,
    __out_bcount(ProcessInformationLength) PVOID ProcessInformation,
    __in ULONG ProcessInformationLength,
    __out_opt PULONG ReturnLength
);

PETHREAD
PsGetNextProcessThread(
    IN PEPROCESS Process,
    IN PETHREAD Thread
);

NTSTATUS
ZwQueryInformationThread(
    __in HANDLE ThreadHandle,
    __in THREADINFOCLASS ThreadInformationClass,
    __out_bcount(ThreadInformationLength) PVOID ThreadInformation,
    __in ULONG ThreadInformationLength,
    __out_opt PULONG ReturnLength
);

NTSTATUS
ZwOpenThread(
    __out PHANDLE ThreadHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PCLIENT_ID ClientId
);

typedef NTSTATUS(__stdcall* PspTerminateThreadByPointer)(IN PETHREAD Thread,
    IN NTSTATUS ExitStatus,
    IN BOOLEAN bSelf);



/*=============================派遣函数=============================*/
//驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

//驱动卸载
VOID Unload(IN PDRIVER_OBJECT DriverObject);

//创建
NTSTATUS DispatchCreate(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);
    
//关闭
NTSTATUS DispatchClose(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);

//控制
NTSTATUS DispatchControl(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);

/*=============================功能函数=============================*/

//遍历进程
NTSTATUS EnumProcess(PVOID pOutBuf, ULONG nBufSize, PVOID pNumberOfBytesRead);

//遍历文件
NTSTATUS EnumFile(PVOID pInBuf, FILE_INFO* pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead);

//遍历注册表
NTSTATUS EnumRegister(ULONG nType, PVOID pInBuf, PVOID pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead);

//获取当前用户SID
NTSTATUS GetCurSid(PVOID pOutBuf, ULONG nBufSize, PULONG pNumberOfBytesRead);

//删除文件
NTSTATUS DeleteFile(PVOID pInBuf);

//删除注册表变量
NTSTATUS DeleteRegVal(PVOID pInBuf);

//删除注册表项
NTSTATUS DeleteRegKey(PVOID pInBuf);

//遍历驱动模块
NTSTATUS EnumDriver(DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//遍历GDT表
NTSTATUS EnumGDT(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//获取CR3
NTSTATUS GetCr3(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//遍历物理内存
NTSTATUS EnumMemPage(ULONG nType, PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutSize, PULONG pNumberOfBytes);

//判断是否为PAE模式
NTSTATUS IsPAEMode(PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//读内存
NTSTATUS ReadProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//写内存
NTSTATUS WriteProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//获取系统主版本号
ULONG GetSystemMajorVersion();

//遍历内存
NTSTATUS EnumMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//中序遍历VAD树
VOID EnumVadTree(PVOID pNode, PULONG puNodeCnt, ULONG uLastTreeLevel, MEM_INFO* pOutBuf);

//遍历驱动模块
NTSTATUS GetSysModule(PDRIVER_OBJECT DriverObject, PVOID pInBuf, ULONG nInSize, DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//读内存
NTSTATUS ReadMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//读取指定文件数据(用来解决一些没权限的文件)
NTSTATUS ReadFileData(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//文件解锁
NTSTATUS UnLockFile(PVOID pInBuf, ULONG nInSize);

//文件粉碎
NTSTATUS FD_SetFileCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
);
HANDLE  FD_OpenFile(WCHAR szFileName[]);
BOOLEAN FD_StripFileAttributes(HANDLE FileHandle);
BOOLEAN FD_DeleteFile(HANDLE FileHandle);
BOOLEAN ForceDeleteFile(WCHAR szFileName[]);
NTSTATUS SmashFile(PVOID pInBuf, ULONG nInSize);

//遍历进程中的线程
NTSTATUS EnumThread(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//强制结束进程
NTSTATUS TerminateProcess(PVOID pInBuf, ULONG nInSize);

//遍历系统回调
NTSTATUS EnumSysCallback(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//测试用函数
NTSTATUS Test();