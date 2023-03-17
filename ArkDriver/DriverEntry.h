#pragma once

//#include <ntddk.h>
#include <ntifs.h>
#include <intrin.h>
#include "..\Common\Define.h"
/*=============================���뺯������=============================*/
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



//���������Ϣ����Ҫ�õ��������ṹ��
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

//������Ϣ�ṹ��  
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
    ULONG Count;//�ں����Լ��ص�ģ��ĸ���
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;		//����PID
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;		//�������
    UCHAR HandleAttributes;		//��ȫ����
    USHORT HandleValue;			//���ֵ
    PVOID Object;					//�������
    ULONG GrantedAccess;			//�þ���ķ���Ȩ��
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;		//�������
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];	//�����Ϣ
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



/*=============================��ǲ����=============================*/
//�������
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

//����ж��
VOID Unload(IN PDRIVER_OBJECT DriverObject);

//����
NTSTATUS DispatchCreate(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);
    
//�ر�
NTSTATUS DispatchClose(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);

//����
NTSTATUS DispatchControl(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp);

/*=============================���ܺ���=============================*/

//��������
NTSTATUS EnumProcess(PVOID pOutBuf, ULONG nBufSize, PVOID pNumberOfBytesRead);

//�����ļ�
NTSTATUS EnumFile(PVOID pInBuf, FILE_INFO* pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead);

//����ע���
NTSTATUS EnumRegister(ULONG nType, PVOID pInBuf, PVOID pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead);

//��ȡ��ǰ�û�SID
NTSTATUS GetCurSid(PVOID pOutBuf, ULONG nBufSize, PULONG pNumberOfBytesRead);

//ɾ���ļ�
NTSTATUS DeleteFile(PVOID pInBuf);

//ɾ��ע������
NTSTATUS DeleteRegVal(PVOID pInBuf);

//ɾ��ע�����
NTSTATUS DeleteRegKey(PVOID pInBuf);

//��������ģ��
NTSTATUS EnumDriver(DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//����GDT��
NTSTATUS EnumGDT(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//��ȡCR3
NTSTATUS GetCr3(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//���������ڴ�
NTSTATUS EnumMemPage(ULONG nType, PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutSize, PULONG pNumberOfBytes);

//�ж��Ƿ�ΪPAEģʽ
NTSTATUS IsPAEMode(PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//���ڴ�
NTSTATUS ReadProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//д�ڴ�
NTSTATUS WriteProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//��ȡϵͳ���汾��
ULONG GetSystemMajorVersion();

//�����ڴ�
NTSTATUS EnumMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//�������VAD��
VOID EnumVadTree(PVOID pNode, PULONG puNodeCnt, ULONG uLastTreeLevel, MEM_INFO* pOutBuf);

//��������ģ��
NTSTATUS GetSysModule(PDRIVER_OBJECT DriverObject, PVOID pInBuf, ULONG nInSize, DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead);

//���ڴ�
NTSTATUS ReadMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//��ȡָ���ļ�����(�������һЩûȨ�޵��ļ�)
NTSTATUS ReadFileData(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//�ļ�����
NTSTATUS UnLockFile(PVOID pInBuf, ULONG nInSize);

//�ļ�����
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

//���������е��߳�
NTSTATUS EnumThread(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//ǿ�ƽ�������
NTSTATUS TerminateProcess(PVOID pInBuf, ULONG nInSize);

//����ϵͳ�ص�
NTSTATUS EnumSysCallback(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes);

//�����ú���
NTSTATUS Test();