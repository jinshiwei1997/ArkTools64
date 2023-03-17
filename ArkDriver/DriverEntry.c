#include "DriverEntry.h"

//驱动入口
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status = STATUS_SUCCESS;		//返回值
    //注册卸载函数
    DriverObject->DriverUnload = Unload;
    
    //创建设备对象
    UNICODE_STRING ustrDeivceName = { 0 };
    RtlInitUnicodeString(&ustrDeivceName, DEVICE_NAME);
    PDEVICE_OBJECT pDeviceObj = NULL;
    status = IoCreateDevice(DriverObject,
        0,
        &ustrDeivceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &pDeviceObj);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[test] IoCreateDevice Error Status:%p\r\n", status));
    }
    else {
        KdPrint(("[test] IoCreateDevice OK Status:%p pDeviceObj:%p\r\n", status, pDeviceObj));
    }

    //符号链接
    UNICODE_STRING ustrSymbolName = { 0 };
    RtlInitUnicodeString(&ustrSymbolName, SYMBOL_NAME);
    status = IoCreateSymbolicLink(&ustrSymbolName, &ustrDeivceName);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[test] IoCreateSymbolicLink Error Status:%p\r\n", status));
        IoDeleteDevice(pDeviceObj);
    }
    else {
        KdPrint(("[test] IoCreateSymbolicLink OK Status:%p\r\n", status));
    }

    //注册派遣函数
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;


    //Test();

    KdPrint(("[test] Hello Driver!\r\n"));		//输出日志
    return status;		//返回
}

//卸载函数
VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
    if (DriverObject->DeviceObject != NULL)
    {
        IoDeleteDevice(DriverObject->DeviceObject);
        UNICODE_STRING ustrSymbolName = { 0 };
        RtlInitUnicodeString(&ustrSymbolName, SYMBOL_NAME);
        IoDeleteSymbolicLink(&ustrSymbolName);
    }
    KdPrint(("[test] Unload\r\n"));
}

//创建
NTSTATUS DispatchCreate(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    KdPrint(("[test] DispatchCreate\r\n"));
    return STATUS_SUCCESS;
}

//关闭
NTSTATUS DispatchClose(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    KdPrint(("[test] DispatchClose\r\n"));
    return STATUS_SUCCESS;
}

//控制
NTSTATUS DispatchControl(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp)
{   
    UNREFERENCED_PARAMETER(DeviceObject);
    ULONG NumberOfBytes = 0;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    //获取IRP堆栈
    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(Irp);

    //获取控制码、缓冲区大小、缓冲区
    ULONG nIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;		//控制码
    ULONG nOutBufLen = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;	//输出缓冲区大小
    ULONG nInBufLen = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    PVOID pSystemBuffer = Irp->AssociatedIrp.SystemBuffer;			//缓冲区

    switch (nIoControlCode)
    {
    case ENUM_PROCESS:
        //KdPrint(("[test] %s: case ENUMPROCESS:\r\n", __FUNCTION__));
        status = EnumProcess(pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ENUM_FILE:
        //KdPrint(("[test] %s: case ENUMFILE:\r\n", __FUNCTION__));
        status = EnumFile(pSystemBuffer, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ENUM_REGISTER_SIZE:
    case ENUM_REGISTER_KEY:
    case ENUM_REGISTER_VAL:
        //KdPrint(("[test] %s: case ENUMREGISTER:%ws\r\n", __FUNCTION__, pSystemBuffer));
        status = EnumRegister(nIoControlCode, pSystemBuffer, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case CUR_USER_PATH:
        //KdPrint(("[test] %s: case CURUSERPATH:\r\n", __FUNCTION__));
        status = GetCurSid(pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case DELETE_FILE:
        //KdPrint(("[test] %s: case DELETEFILE:\r\n", __FUNCTION__));
        status = DeleteFile(pSystemBuffer);
        break;
    case DELETE_REG_KEY:
        //KdPrint(("[test] %s: case DELETEREG_KEY:\r\n", __FUNCTION__));
        status = DeleteRegKey(pSystemBuffer);
        break;
    case DELETE_REG_VAL:
        //KdPrint(("[test] %s: case DELETEREG_VAL:\r\n", __FUNCTION__));
        status = DeleteRegVal(pSystemBuffer);
        break;
    case ENUM_DRIVER:
        //KdPrint(("[test] %s: case DELETEREG_VAL:\r\n", __FUNCTION__));
        status = EnumDriver(pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ENUM_GDT:
        status = EnumGDT(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case GET_CR3:
        status = GetCr3(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ENUM_PAGE_101012:
    case ENUM_PAGE_29912:
    case ENUM_PAGE_29912_PDPTE:
        status = EnumMemPage(nIoControlCode, pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ISPAE:
        status = IsPAEMode(pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case READ_PROC_MEMORY:
        status = ReadProcMemory(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case WRITE_PROC_MEMORY:
        status = WriteProcMemory(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case ENUM_MEMORY:
        status = EnumMemory(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case READ_SYS_MEMORY:
        status = ReadMemory(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case READ_FILE:
        status = ReadFileData(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case FILE_UNLOCK:
        status = UnLockFile(pSystemBuffer, nInBufLen);
        break;
    case FILE_SMASH:
        status = SmashFile(pSystemBuffer, nInBufLen);
        break;
    case ENUM_THREAD:
        status = EnumThread(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case TERMINATE_PROCESS:
        status = TerminateProcess(pSystemBuffer, nInBufLen);
        break;
    case ENUM_SYSCALLBACK:
        status = EnumSysCallback(pSystemBuffer, nInBufLen, pSystemBuffer, nOutBufLen, &NumberOfBytes);
        break;
    case TEST:
        status = Test();
        break;
    }

    Irp->IoStatus.Status = status;	//函数执行状态, GetLastError()获取的就是这个值
    Irp->IoStatus.Information = NumberOfBytes;	//对应lpNumberOfBytesRead参数, 也就是读取字节数
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

//遍历进程
NTSTATUS EnumProcess(PVOID pOutBuf, ULONG nBufSize, PVOID pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_SUCCESS;
    PVOID pBuf = NULL;
    ULONG nBufLength = 0;
    PSYSTEM_PROCESSES pProcInfo = NULL;
    PEPROCESS pProcess = NULL;
    PROCESS_INFO* pInfoAry = pOutBuf;
    ULONG uCurSize = 0;

    UNICODE_STRING ustrName = { 0 };
    PUNICODE_STRING pustrPath = NULL;
    ULONG uRetLength = 0;

    HANDLE hProcessHandle = NULL;
    OBJECT_ATTRIBUTES obj = { 0 };
    CLIENT_ID clientid = { 0 };
    InitializeObjectAttributes(&obj, 0, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);

    KdPrint(("[test] %s - nBufSize:%d\r\n", __FUNCTION__, nBufSize));

    /*获取进程信息*/
    status = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &nBufLength);
    if (nBufLength == 0)
    {
        KdPrint(("[test] %s - 1.ZwQuerySystemInformation Error status:%p\r\n", __FUNCTION__, status));
        return status;
    }
    else 
    {
#pragma warning(suppress : 4996)
        pBuf = ExAllocatePool(NonPagedPool, nBufLength);
        if (pBuf == NULL)
        {
            KdPrint(("[test] ExAllocatePool Error\r\n"));
            goto ENUM_PROC_EXIT;
        }
        pProcInfo = pBuf;
        status = ZwQuerySystemInformation(SystemProcessInformation, pBuf, nBufLength, &nBufLength);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("[test] %s - 2.ZwQuerySystemInformation Error status:%p\r\n", __FUNCTION__, status));
            goto ENUM_PROC_EXIT;
        }
        while (TRUE)
        {
            if (pProcInfo->UniqueProcessId != 0)
            {
                if (nBufSize != 0)
                {
                    status = PsLookupProcessByProcessId((HANDLE)pProcInfo->UniqueProcessId, &pProcess);
                    if (!NT_SUCCESS(status))
                    {
                        KdPrint(("[test] %s - PsLookupProcessByProcessId Error status:%p\r\n", __FUNCTION__, status));
                        goto ENUM_PROC_EXIT;
                    }

                    if (pProcInfo->ImageName.Buffer == NULL)
                    {
                        RtlInitUnicodeString(&ustrName, L"null");
                    }
                    else
                    {
                        RtlInitUnicodeString(&ustrName, pProcInfo->ImageName.Buffer);
                    }

                    pustrPath = ExAllocatePoolWithTag(NonPagedPool, sizeof(WCHAR) * 1024, 1);
                    RtlZeroMemory(pustrPath, sizeof(WCHAR) * 1024);
                    uRetLength = 0;
                    if (pProcInfo->UniqueProcessId == (HANDLE)4)
                    {
                        RtlInitUnicodeString(pustrPath, L"System");
                    }
                    else
                    {
                        clientid.UniqueProcess = pProcInfo->UniqueProcessId;
                        status = ZwOpenProcess(&hProcessHandle, PROCESS_ALL_ACCESS, &obj, &clientid);
                        if (!NT_SUCCESS(status))
                        {
                            KdPrint(("[test] %s - ZwOpenProcess Error status=%p, PID=%d\r\n", __FUNCTION__, status, pProcInfo->UniqueProcessId));
                            ObReferenceObject(pProcess);
                            goto ENUM_PROC_EXIT;
                        }
                        status = ZwQueryInformationProcess(hProcessHandle, ProcessImageFileName, pustrPath, sizeof(WCHAR) * 1024, &uRetLength);
                        if (!NT_SUCCESS(status))
                        {
                            KdPrint(("[test] %s - ZwQueryInformationProcess Error status=%p, PID=%d\r\n", __FUNCTION__, status, pProcInfo->UniqueProcessId));
                            ObReferenceObject(pProcess);
                            ZwClose(hProcessHandle);
                            goto ENUM_PROC_EXIT;
                        }
                    }
                    pInfoAry->m_nPid = pProcInfo->UniqueProcessId;
                    pInfoAry->m_nInheritedPid = pProcInfo->InheritedFromUniqueProcessId;
                    pInfoAry->m_pEPROCESS = pProcess;
                    RtlCopyMemory(pInfoAry->m_szName, ustrName.Buffer, ustrName.MaximumLength);
                    RtlCopyMemory(pInfoAry->m_szPatch, pustrPath->Buffer,
                        pustrPath->MaximumLength < sizeof(pInfoAry->m_szPatch) ? pustrPath->MaximumLength : sizeof(pInfoAry->m_szPatch) - sizeof(WCHAR));
                    ExFreePool(pustrPath);
                    pustrPath = NULL;
                    pInfoAry++;
                    ObReferenceObject(pProcess);
                }
                uCurSize += sizeof(PROCESS_INFO);
            }
            if (pProcInfo->NextEntryOffset == 0 || (uCurSize >= nBufSize && nBufSize != 0))
            {
                break;
            }
            pProcInfo = (PSYSTEM_PROCESSES)((PCHAR)pProcInfo + pProcInfo->NextEntryOffset);
        }
        *(PULONG)pNumberOfBytesRead = uCurSize;
        ExFreePool(pBuf);
        pBuf = NULL;
    }
ENUM_PROC_EXIT:
    if (pBuf != NULL)
    {
        ExFreePool(pBuf);
    }
    if (pustrPath != NULL)
    {
        ExFreePool(pustrPath);
    }
    return status;
}

//遍历文件
NTSTATUS EnumFile(PVOID pInBuf, FILE_INFO* pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hFile = NULL;
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    IO_STATUS_BLOCK IoStatusBlock = { 0 };
    PFILE_BOTH_DIR_INFORMATION pFileInfo = NULL;
    ULONG nReadSize = 0;
    UNICODE_STRING ustrPath = { 0 };
    UNICODE_STRING us1;
    UNICODE_STRING us2;
    RtlInitUnicodeString(&ustrPath, pInBuf);
    RtlInitUnicodeString(&us1, L".");
    RtlInitUnicodeString(&us2, L"..");
    ULONG nBufSize = 65535 * 2 * 2;
#pragma warning(suppress : 4996)
    PVOID pBuf = ExAllocatePool(NonPagedPool, nBufSize);
    if (pBuf == NULL)
    {
        KdPrint(("[test] %s:ExAllocatePool Error\r\n", __FUNCTION__));
        return -1;
    }
    
    //初始化对象属性
    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    status = ZwCreateFile(&hFile,   //句柄                       
        FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_ANY_ACCESS,        //访问权限
        &ObjAttributes,             //对象属性  
        &IoStatusBlock,             //返回状态信息
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,     //共享访问属性
        FILE_OPEN,                  //指定文件不存在时做的操作
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,   //打开方式
        NULL, 0);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s:ZwCreateFile Error, status:%p, FilePath:%ws\r\n", __FUNCTION__, status, ustrPath.Buffer));
        goto FILE_EXIT;
    }
    //遍历
    status = ZwQueryDirectoryFile(hFile,
        NULL, NULL, NULL,
        &IoStatusBlock,                         //返回状态信息
        pBuf, nBufSize,            //缓冲区
        FileBothDirectoryInformation,           //信息类型
        FALSE, NULL, FALSE);
    if (!NT_SUCCESS(status))
    {
        if (status == STATUS_BUFFER_OVERFLOW)
        {
            KdPrint(("[test] %s:1遍历文件所需大小%d\r\n", __FUNCTION__, IoStatusBlock.Information));
        }
        else
        {
            KdPrint(("[test] %s:ZwQueryDirectoryFile Error, status:%p\r\n", __FUNCTION__, status));
        }

        goto FILE_EXIT;
    }

    pFileInfo = pBuf;
    while (TRUE)
    {
        BOOLEAN b1 = RtlCompareMemory(pFileInfo->FileName, us1.Buffer, us1.Length) == us1.Length;
        BOOLEAN b2 = RtlCompareMemory(pFileInfo->FileName, us1.Buffer, us1.Length) == us1.Length;
        ULONG nLen = pFileInfo->FileNameLength;
        if (!((b1 && nLen == us1.Length) || (b2 && nLen == us2.Length)))
        {
            RtlCopyMemory(pOutBuf->m_szName, pFileInfo->FileName, pFileInfo->FileNameLength);
            pOutBuf->m_szName[pFileInfo->FileNameLength / 2] = 0;
            pOutBuf->m_nFileAttributes = pFileInfo->FileAttributes;
            pOutBuf->m_nFileSize = pFileInfo->AllocationSize;
            pOutBuf->m_nCreateTime = pFileInfo->CreationTime;
            pOutBuf->m_nChangeTime = pFileInfo->ChangeTime;
            nReadSize += sizeof(FILE_INFO);
            pOutBuf++;
        }

        if (pFileInfo->NextEntryOffset == 0 || nReadSize >= nOutBufSize)
        {
            break;
        }
        pFileInfo = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)pFileInfo + pFileInfo->NextEntryOffset);
    }
    *(PULONG)pNumberOfBytesRead = nReadSize;
    KdPrint(("[test] %s: pNumberOfBytesRead:%d\r\n", __FUNCTION__, nReadSize));

FILE_EXIT:
    if (pBuf != NULL)
    {
        ExFreePool(pBuf);
    }
    if (hFile != NULL)
    {
        status = ZwClose(hFile);
    }
    return status;
}

//遍历注册表
NTSTATUS EnumRegister(ULONG nType, PVOID pInBuf, PVOID pOutBuf, ULONG nOutBufSize, PVOID pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hKey = NULL;
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    UNICODE_STRING ustrPath;
    ULONG nWriteBytes = 0;
    ULONG nRetBytes = 0;
    PVOID pInfo = NULL;
    PVOID pBuf = NULL;
    PKEY_FULL_INFORMATION pKeyInfo = NULL;
    PKEY_FULL_INFORMATION pSubKeyInfo = NULL;
    PKEY_BASIC_INFORMATION pKeyNameInfo = NULL;
    PKEY_VALUE_FULL_INFORMATION pValInfo = NULL;
    REG_SIZE_INFO* pOutSizeInfo = pOutBuf;
    REG_KEY_INFO* pOutKeyInfo = pOutBuf;
    REG_VAL_INFO* pOutValInfo = pOutBuf;

    //初始化对象属性
    RtlInitUnicodeString(&ustrPath, (PCWSTR)pInBuf);

    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    //获取句柄
    status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttributes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwOpenKey Error, status:%p, path:%ws\r\n", __FUNCTION__, status, ustrPath.Buffer));
        goto REG_EXIT;
    }

    RtlZeroMemory(pOutBuf, nOutBufSize);    //由于输入输出共占同一片缓冲区，所以要先使用完输入数据后在清空缓冲区

    //获取缓冲区大小
    status = ZwQueryKey(hKey, KeyFullInformation, NULL, 0, &nRetBytes);
    if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL)
    {
        //KdPrint(("[test] %s: ZwQueryKey NeedBuffSize = %d，%d\r\n", __FUNCTION__, nRetBytes, sizeof(KEY_FULL_INFORMATION));
    }
    else if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: 1.ZwQueryKey Error, status:%p\r\n", __FUNCTION__, status));
        goto REG_EXIT;
    }

    //申请缓冲区
#pragma warning(suppress : 4996)
    pInfo = ExAllocatePool(NonPagedPool, nRetBytes);
    if (pInfo == NULL)
    {
        KdPrint(("[test] %s: ExAllocatePool Error\r\n", __FUNCTION__));
        status = -1;
        goto REG_EXIT;
    }

    //获取信息
    status = ZwQueryKey(hKey, KeyFullInformation, pInfo, nRetBytes, &nRetBytes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: 2.ZwQueryKey Error, status:%p\r\n", __FUNCTION__, status));
        goto REG_EXIT;
    }
    pKeyInfo = pInfo;
    if (nType == ENUM_REGISTER_SIZE && nOutBufSize >= sizeof(REG_SIZE_INFO))
    {
        pOutSizeInfo->m_nSubKeys = pKeyInfo->SubKeys;
        pOutSizeInfo->m_nValues = pKeyInfo->Values;
        //KdPrint(("[test] %s: SubKeys=%d, Values=%d\r\n", __FUNCTION__,
        //    pOutSizeInfo->m_nSubKeys, pOutSizeInfo->m_nValues));
        nWriteBytes += sizeof(REG_SIZE_INFO);
        *(PULONG)pNumberOfBytesRead = nWriteBytes;
        goto REG_EXIT;
    }

    //子项信息 ZwEnumerateKey
    if (nType == ENUM_REGISTER_KEY)
    {
        nWriteBytes = 0;
        for (ULONG i = 0; i < pKeyInfo->SubKeys; i++)
        {
            /*获取子项信息(用于判断子项是否还拥有子项)*/
            nRetBytes = 0;
            status = ZwEnumerateKey(hKey, i, KeyFullInformation, NULL, 0, &nRetBytes);      //获取大小
            if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL)
            {
                //KdPrint(("[test] %s: ZwEnumerateKey-KeyFullInformation(sub) NeedBuffSize = %d, LoopCnt:%d\r\n", __FUNCTION__, nRetBytes, i));
            }
            else if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: 1.ZwEnumerateKey-KeyFullInformation(sub) Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                continue;
            }
#pragma warning(suppress : 4996)
            pBuf = ExAllocatePool(NonPagedPool, nRetBytes);     //申请缓冲区
            if (pInfo == NULL)
            {
                KdPrint(("[test] %s: EnumKey ->1.ExAllocatePool Error, LoopCnt:%d\r\n", __FUNCTION__, i));
                continue;
            }
            status = ZwEnumerateKey(hKey, i, KeyFullInformation, pBuf, nRetBytes, &nRetBytes);  //获取信息
            if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: 2.ZwEnumerateKey-KeyFullInformation(sub) Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                goto ENUM_KEY_EXIT;
            }
            pSubKeyInfo = pBuf;
            pOutKeyInfo->m_nSubKeys = pSubKeyInfo->SubKeys;
            ExFreePool(pBuf);       //释放缓冲区

            /*获取子项名称*/
            nRetBytes = 0;
            status = ZwEnumerateKey(hKey, i, KeyBasicInformation, NULL, 0, &nRetBytes);      //获取大小
            if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL)
            {
                //KdPrint(("[test] %s: ZwEnumerateKey-KeyBasicInformation NeedBuffSize = %d, LoopCnt:%d\r\n", __FUNCTION__, nRetBytes, i));
            }
            else if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: 1.ZwEnumerateKey-KeyBasicInformation Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                continue;
            }
#pragma warning(suppress : 4996)
            pBuf = ExAllocatePool(NonPagedPool, nRetBytes);     //申请缓冲区
            if (pInfo == NULL)
            {
                KdPrint(("[test] %s: EnumKey ->2.ExAllocatePool Error, LoopCnt:%d\r\n", __FUNCTION__, i));
                continue;
            }
            status = ZwEnumerateKey(hKey, i, KeyBasicInformation, pBuf, nRetBytes, &nRetBytes);  //获取信息
            if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: 2.ZwEnumerateKey-KeyBasicInformation Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                goto ENUM_KEY_EXIT;
            }
            pKeyNameInfo = pBuf;
            RtlCopyMemory(pOutKeyInfo->m_szKeyName, pKeyNameInfo->Name, pKeyNameInfo->NameLength);
            pOutKeyInfo->m_nNameLen = pKeyNameInfo->NameLength;
            //KdPrint(("[test] %s: SubKeyName:%ws\r\n", __FUNCTION__, pOutKeyInfo->m_szKeyName));
            pOutKeyInfo++;
            nWriteBytes += sizeof(REG_KEY_INFO);

        ENUM_KEY_EXIT:
            ExFreePool(pBuf);
            if (nWriteBytes >= nOutBufSize)
            {
                break;
            }
        }
        KdPrint(("[test] %s: BufSize:%d, WriteSize:%d\r\n", __FUNCTION__, nOutBufSize, nWriteBytes));
        *(PULONG)pNumberOfBytesRead = nWriteBytes;
        goto REG_EXIT;
    }

    //变量信息 ZwEnumerateValueKey
    if (nType == ENUM_REGISTER_VAL)
    {
        nWriteBytes = 0;
        for (ULONG i = 0; i < pKeyInfo->Values; i++)
        {
            nRetBytes = 0;
            status = ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, NULL, 0, &nRetBytes);      //获取大小
            if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL)
            {
                //KdPrint(("[test] %s: ZwEnumerateValueKey NeedBuffSize = %d, LoopCnt:%d\r\n", __FUNCTION__, nRetBytes, i));
            }
            else if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: ZwEnumerateValueKey Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                continue;
            }

#pragma warning(suppress : 4996)
            pBuf = ExAllocatePool(NonPagedPool, nRetBytes);     //申请缓冲区
            if (pInfo == NULL)
            {
                KdPrint(("[test] %s: EnumVal->1.ExAllocatePool Error, LoopCnt:%d\r\n", __FUNCTION__, i));
                continue;
            }

            status = ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, pBuf, nRetBytes, &nRetBytes);        //获取信息
            if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: 2.ZwEnumerateValueKey Error, status:%p, LoopCnt:%d\r\n", __FUNCTION__, status, i));
                goto ENUM_VAL_EXIT;
            }
            pValInfo = pBuf;
            pOutValInfo->m_nValType = pValInfo->Type;
            pOutValInfo->m_nNameLen = pValInfo->NameLength;
            RtlCopyMemory(pOutValInfo->m_szValName, pValInfo->Name, pValInfo->NameLength);
            pOutValInfo->m_nDataLen = pValInfo->DataLength;
            RtlCopyMemory(pOutValInfo->m_szValData, (PCHAR)pBuf + pValInfo->DataOffset, pValInfo->DataLength < 4096 ? pValInfo->DataLength : 4096);
            //KdPrint(("[test] %s: ValName:%ws, ValSize:%d, BufSize:%d\r\n", __FUNCTION__, pOutValInfo->m_szValName, pValInfo->DataLength, nOutBufSize));
            pOutValInfo++;
            nWriteBytes += sizeof(REG_VAL_INFO);
        

        ENUM_VAL_EXIT:
            ExFreePool(pBuf);
            if (nWriteBytes >= nOutBufSize)
            {
                break;
            }
        }
        *(PULONG)pNumberOfBytesRead = nWriteBytes;
        goto REG_EXIT;
    }


REG_EXIT:
    if (pInfo != NULL)
    {
        ExFreePool(pInfo);
    }
    if (hKey != NULL)
    {
        ZwClose(hKey);
    }
    return status;
}

//获取当前SID(返回注册表路径，需要在手动剔除前面的路径)
NTSTATUS GetCurSid(PVOID pOutBuf, ULONG nBufSize, PULONG pNumberOfBytesRead)
{
    nBufSize;
    NTSTATUS status = STATUS_SUCCESS;
    PVOID pKeyBuf = NULL;
    PVOID pValBuf = NULL;
    ULONG nRetBytes = 0;
    REG_SIZE_INFO RegSizeInfo = { 0 };
    REG_SIZE_INFO SubSizeInfo = { 0 };
    REG_KEY_INFO* pKey = NULL;
    REG_VAL_INFO* pVal = NULL; pVal;
    ULONG* pnData = NULL; pnData;
    UNICODE_STRING ustrPath;
    UNICODE_STRING ustrValName;
    WCHAR CurrentUserbuf[260] = L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\";
    RtlInitEmptyUnicodeString(&ustrPath, CurrentUserbuf, 260 * sizeof(WCHAR));
    ustrPath.Length = sizeof(L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\") - 2;
    RtlInitUnicodeString(&ustrValName, L"RefCount");

    status = EnumRegister(ENUM_REGISTER_SIZE, ustrPath.Buffer, &RegSizeInfo, sizeof(RegSizeInfo), &nRetBytes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: 1.ENUMREGISTER_SIZE Error, status:%p\r\n", __FUNCTION__, status));
        goto SID_EXIT;
    }
#pragma warning(suppress : 4996)
    pKeyBuf = ExAllocatePool(NonPagedPool, RegSizeInfo.m_nSubKeys * sizeof(REG_KEY_INFO));
    status = EnumRegister(ENUM_REGISTER_KEY, ustrPath.Buffer, pKeyBuf, RegSizeInfo.m_nSubKeys * sizeof(REG_KEY_INFO), &nRetBytes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: 2.ENUMREGISTER_KEY Error, status:%p\r\n", __FUNCTION__, status));
        goto SID_EXIT;
    }

    pKey = pKeyBuf;
    for (ULONG i = 0; i < RegSizeInfo.m_nSubKeys; i++)
    {
        if (pKey[i].m_nNameLen > 20)
        {
            RtlAppendUnicodeToString(&ustrPath, pKey[i].m_szKeyName);
            status = EnumRegister(ENUM_REGISTER_SIZE, ustrPath.Buffer, &SubSizeInfo, sizeof(SubSizeInfo), &nRetBytes);
            if (!NT_SUCCESS(status))
            {
                continue;
            }
#pragma warning(suppress : 4996)
            pValBuf = ExAllocatePool(NonPagedPool, SubSizeInfo.m_nValues * sizeof(REG_VAL_INFO));
            status = EnumRegister(ENUM_REGISTER_VAL, ustrPath.Buffer, pValBuf, SubSizeInfo.m_nValues * sizeof(REG_VAL_INFO), &nRetBytes);
            if (NT_SUCCESS(status))
            {
                pVal = pValBuf;
                //KdPrint(("[test] %s: m_nValues=%d\r\n", __FUNCTION__, SubSizeInfo.m_nValues));
                for (ULONG j = 0; j < SubSizeInfo.m_nValues; j++)
                {
                    SIZE_T nRet = RtlCompareMemory(pVal[j].m_szValName, ustrValName.Buffer, ustrValName.Length);
                    //KdPrint(("[test] %s: %ws, Ret=%d, Len=%d,%d\r\n", __FUNCTION__, pVal[j].m_szValName, nRet, ustrValName.Length, pVal[j].m_nNameLen));
                    pnData = (ULONG*)pVal[j].m_szValData;
                    if (nRet == ustrValName.Length && *pnData > 0)
                    {
                        KdPrint(("[test] %s: CurUser=%ws\r\n", __FUNCTION__, ustrPath.Buffer));
                        RtlCopyMemory(pOutBuf, ustrPath.Buffer, ustrPath.MaximumLength);
                        *pNumberOfBytesRead = ustrPath.MaximumLength;
                        ExFreePool(pValBuf);
                        goto SID_EXIT;
                        break;
                    }
                    pnData = NULL;
                }
            }
            ExFreePool(pValBuf);
        }
    }

SID_EXIT:
    if (pKeyBuf != NULL)
        ExFreePool(pKeyBuf);
    return status;
}

//删除文件
NTSTATUS DeleteFile(PVOID pInBuf)
{
    NTSTATUS status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    UNICODE_STRING ustrPath;

    RtlInitUnicodeString(&ustrPath, pInBuf);
    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);
    status = ZwDeleteFile(&ObjAttributes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s  ZwDeleteFile Error, status:%p\r\n", __FUNCTION__, status));
    }

    return status;
}

//删除注册表变量
NTSTATUS DeleteRegVal(PVOID pInBuf)
{    
    NTSTATUS status = STATUS_SUCCESS;
    REG_VAL_DELETE* prvd = pInBuf;
    UNICODE_STRING ustrPath = { 0 };
    UNICODE_STRING ustrName = { 0 };
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    HANDLE hKey = NULL;

    RtlInitUnicodeString(&ustrPath, prvd->m_szRegPath);
    RtlInitUnicodeString(&ustrName, prvd->m_szValName);

    //初始化对象属性
    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    //获取句柄
    status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttributes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwOpenKey Error, status:%p, path:%ws\r\n", __FUNCTION__, status, ustrPath.Buffer));
        return status;
    }

    //删除变量
    status = ZwDeleteValueKey(hKey, &ustrName);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwDeleteValueKey Error, status:%p, path:%ws, ValName:%ws\r\n", __FUNCTION__, status, ustrPath.Buffer, ustrName.Buffer));
    }

    if (hKey != NULL)
    {
        ZwClose(hKey);
    }
    return status;
}

//删除注册表项
NTSTATUS DeleteRegKey(PVOID pInBuf)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING ustrPath = { 0 };
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    HANDLE hKey = NULL;

    RtlInitUnicodeString(&ustrPath, pInBuf);

    //初始化对象属性
    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    //获取句柄
    status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttributes);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwOpenKey Error, status:%p\r\n", __FUNCTION__, status));
        return status;
    }

    //删除
    status = ZwDeleteKey(hKey);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwDeleteKey Error, status:%p\r\n", __FUNCTION__, status));
    }
    else
    {
        KdPrint(("[test] %s: KeyDelete Ok\r\n", __FUNCTION__, status));
    }


    ZwClose(hKey);
    return status;
}

NTSTATUS EnumDriver(DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    ULONG uBufSize = 0;
    ULONG uRetBytes = 0;
    ULONG uCurReadBytes = 0;
    PSYSTEM_MODULE_INFORMATION psmi = NULL;
    WCHAR ch = 0;
    
    //获取缓冲区大小
    status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &uRetBytes);
    if (!NT_SUCCESS(status) && status != STATUS_INFO_LENGTH_MISMATCH)
    {
        KdPrint(("[test] %s: ZwQuerySystemInformation, GetBufLength Err\r\n", __FUNCTION__, status));
        return status;
    }
    uBufSize = uRetBytes;

    psmi = ExAllocatePoolWithTag(NonPagedPool, uBufSize, 1);
    __try
    {
        //获取信息
        status = ZwQuerySystemInformation(SystemModuleInformation, psmi, uBufSize, &uRetBytes);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("[test] %s: ZwQuerySystemInformation GetInfoErr\r\n", __FUNCTION__, status));
            goto ENUM_DRV_EXIT;
        }

        //判断用户缓冲区大小
        if (nOutBufSize == 0)
        {
            uCurReadBytes = psmi->Count * sizeof(DRIVER_INFO);
            goto ENUM_DRV_EXIT;     //如果缓冲区大小为0，则返回所需大小。
        }

        //遍历信息
        for (ULONG i = 0; i < psmi->Count; i++)
        {
            uCurReadBytes += sizeof(DRIVER_INFO);
            if (uCurReadBytes > nOutBufSize)            //判断溢出
            {
                uCurReadBytes -= sizeof(DRIVER_INFO);
                goto ENUM_DRV_EXIT;
            }
            pOutBuf[i].m_nBase = psmi->Module[i].Base;
            pOutBuf[i].m_nSize = psmi->Module[i].Size;
            //复制路径以及名称
            for (ULONG j = 0; psmi->Module[i].ImageName[j] != 0; j++)
            {
                ch = psmi->Module[i].ImageName[j];
                pOutBuf[i].m_szPatch[j] = ch;
                if (j >= psmi->Module[i].ModuleNameOffset)
                {
                    pOutBuf[i].m_szName[j - psmi->Module[i].ModuleNameOffset] = ch;
                }
            }
        }
    }
    __except (1)
    {
        KdPrint(("[test] %s: __except, psmi=%p, sysBuf=%p\r\n", __FUNCTION__, status));
    }

ENUM_DRV_EXIT:
    *pNumberOfBytesRead = uCurReadBytes;
    if (psmi != NULL)
    {
        ExFreePool(psmi);
    }
    return status;
}

NTSTATUS EnumGDT(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_SUCCESS;
    GDT* gdt = (GDT*)pInBuf;

    if (nInSize < sizeof(GDT))
    {
        KdPrint(("[test] %s: nInSize < sizeof(GDT)\r\n", __FUNCTION__));
        return STATUS_UNSUCCESSFUL;
    }

    //检查gdt表地址
    if (!MmIsAddressValid(gdt->SegDescriptor))
    {
        KdPrint(("[test] %s: MmIsAddressValid(gdt->SegDescriptor)\r\n", __FUNCTION__));
        return STATUS_UNSUCCESSFUL;
    }

    //判断输入参数
    if (nOutBufSize <= gdt->Size)
    {
        if (nOutBufSize == 0)
        {
            *pNumberOfBytesRead = gdt->Size + 1;
            return status;
        }
        else
        {
            KdPrint(("[test] %s: nOutBufSize <= gdt->Size\r\n", __FUNCTION__));
            return STATUS_UNSUCCESSFUL;
        }
    }

    if (!MmIsAddressValid(pOutBuf))
    {
        KdPrint(("[test] %s: MmIsAddressValid(pOutBuf)\r\n", __FUNCTION__));
        return STATUS_UNSUCCESSFUL;
    }
    KdPrint(("[test] %s: SegDescriptor=%p, size=%p\r\n", __FUNCTION__, gdt->SegDescriptor, gdt->Size));
    RtlCopyMemory(pOutBuf, gdt->SegDescriptor, gdt->Size + 1);
    *pNumberOfBytesRead = nOutBufSize;
    return status;
}

NTSTATUS GetCr3(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(pInBuf);
    UNREFERENCED_PARAMETER(nInSize);
    UNREFERENCED_PARAMETER(pOutBuf);
    UNREFERENCED_PARAMETER(nOutBufSize);
    UNREFERENCED_PARAMETER(pNumberOfBytes);
    //ULONG regCr3 = 0;
    //HANDLE pid = *(PHANDLE)pInBuf;
    //PVOID pEPROCESS = NULL;
    //PsLookupProcessByProcessId(pid, &(PEPROCESS)pEPROCESS);
    //if (pEPROCESS == NULL)
    //{
    //    KdPrint(("[test] %s: PsLookupProcessByProcessId Err\r\n", __FUNCTION__));
    //    return -1;
    //}
    //regCr3 = *(PULONG)((ULONG)pEPROCESS + 0x18);
    //ObDereferenceObject(pEPROCESS);
    //KdPrint(("[test] %s: Cr3=0x%p\r\n", __FUNCTION__, regCr3));
    //if (nOutBufSize < 4 || nInSize < 4)
    //{
    //    KdPrint(("[test] %s: ERR nInSize=%d, nOutSize=%d\r\n", __FUNCTION__, nInSize, nOutBufSize));
    //    return -1;
    //}
    ////__asm
    ////{
    ////    push eax
    ////    mov eax, cr3
    ////    mov regCr3, eax
    ////    pop eax
    ////}
    //RtlCopyMemory(pOutBuf, &regCr3, 4);
    //*pNumberOfBytes = 4;
    return status;
}

NTSTATUS EnumMemPage(ULONG nType, PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_SUCCESS;
    if (nInSize < 4)
    {
        KdPrint(("[test] %s: nInSize < 4, nInSize=%d\r\n", __FUNCTION__, nInSize));
        return -1;
    }
    PHYSICAL_ADDRESS PhysicsAddr = { 0 };
    PULONG pBuf = NULL;
    ULONG nBufSize = 0;

    if (nType == ENUM_PAGE_101012)
    {
        if (nOutSize < 4096)
        {
            KdPrint(("[test] %s: ENUMPAGE_101012 nOutSize < 4096, nOutSize=%d\r\n", __FUNCTION__, nOutSize));
            return -1;
        }
        PhysicsAddr.LowPart = *(PULONG)pInBuf;
        nBufSize = 1024 * 4;
        
    }
    else if (nType == ENUM_PAGE_29912_PDPTE)
    {
        if (nOutSize < 32)
        {
            KdPrint(("[test] %s: ENUMPAGE_29912_PDPTE nOutSize < 32, nOutSize=%d\r\n", __FUNCTION__, nOutSize));
            return -1;
        }
        PhysicsAddr.LowPart = *(PULONG)pInBuf;      //cr3所以是4字节
        nBufSize = 4 * 8;
    }
    else if (nType == ENUM_PAGE_29912)
    {
        if (nOutSize < 4096)
        {
            KdPrint(("[test] %s: ENUMPAGE_29912 nOutSize < 4096, nOutSize=%d\r\n", __FUNCTION__, nOutSize));
            return -1;
        }
        PhysicsAddr = *(PPHYSICAL_ADDRESS)pInBuf;      
        nBufSize = 512 * 8;
    }

    pBuf = MmMapIoSpace(PhysicsAddr, nBufSize, MmNonCached);
    if (pBuf == NULL)
    {
        KdPrint(("[test] %s: MmMapIoSpaceErr PhysicsAddr=%p nBufSize=%d\r\n", __FUNCTION__, PhysicsAddr,nBufSize));
        return -1;
    }
    RtlCopyMemory(pOutBuf, pBuf, nBufSize);
    MmUnmapIoSpace(pBuf, nBufSize);
    *pNumberOfBytes = nBufSize;
    return status;
}


NTSTATUS IsPAEMode(PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(pOutBuf);
    UNREFERENCED_PARAMETER(nOutBufSize);
    UNREFERENCED_PARAMETER(pNumberOfBytesRead);

    //if (nOutBufSize < 4)
    //{
    //    KdPrint(("[test] %s: nOutBufSize < 4\r\n", __FUNCTION__));
    //    return -1;
    //}
    //SIZE_T CR4 = __readcr4();
   
    //CR4 = CR4 & 0x20;
    //*(PULONG)pOutBuf = CR4;
    //*pNumberOfBytesRead = 4;
    return status;
}

NTSTATUS ReadProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    PAGED_CODE();
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hHandle = ((TARGET_PROC*)pInBuf)->m_hHandle;             //用户传进来的pid
    PVOID pSrcAddr = ((TARGET_PROC*)pInBuf)->m_TargetAddres;        //用户传进来的目标地址
    //ULONG uCr3 = 0;
    //ULONG nRetBytes = 0;

    if (nInSize < sizeof(TARGET_PROC))
    {
        KdPrint(("[test] %s: Err, nInSize < sizeof(TARGET_PROC)\n", __FUNCTION__));
        return STATUS_UNSUCCESSFUL;
    }

    //status = GetCr3(&hHandle, sizeof(hHandle), &uCr3, sizeof(uCr3), &nRetBytes);
    //if (!NT_SUCCESS(status))
    //{
    //    KdPrint(("[test] %s: GetCr3 Err, status=%08X\r\n", __FUNCTION__, status));
    //}
    //__try {
    //    ULONG OldCr3 = __readcr3();
    //    __writecr3((ULONG)uCr3);
    //    RtlCopyMemory(pOutBuf, pSrcAddr, nOutBufSize);
    //    __writecr3(OldCr3);
    //    *pNumberOfBytes = nOutBufSize;
    //}
    //__except (1) {
    //    KdPrint(("[test] %s __except\n", __FUNCTION__));
    //}
    PEPROCESS pEprocess = NULL;
    KAPC_STATE KApcState = { 0 };
    BOOLEAN bAttachFlag = FALSE;
    PHYSICAL_ADDRESS PhysicalAddr = { 0 };
    PVOID pMap = NULL;
    status = PsLookupProcessByProcessId(hHandle, &pEprocess);       //获取EPROCESS
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: PsLookupProcessByProcessId Err, status=%08X\r\n", __FUNCTION__, status));
        return status;
    }
    __try
    {
        KeStackAttachProcess(pEprocess, &KApcState);        //挂靠进程(切换CR3)
        bAttachFlag = TRUE;
        PhysicalAddr = MmGetPhysicalAddress(pSrcAddr);
        pMap = MmMapIoSpace(PhysicalAddr, nOutBufSize, MmNonCached);    //将要读取的地址重新映射一份线性地址
        if (pMap != NULL)
        {
            RtlCopyMemory(pOutBuf, pMap, nOutBufSize);      //读数据
            *pNumberOfBytes = nOutBufSize;
            MmUnmapIoSpace(pMap, nOutBufSize);
            status = STATUS_SUCCESS;
        }
        else
        {
            KdPrint(("[test] %s: MmMapIoSpace Err, pSrcAddr=%p, PhysicalAddr=%p\r\n", __FUNCTION__, pSrcAddr, PhysicalAddr));
            status = STATUS_UNSUCCESSFUL;
        }
        KeUnstackDetachProcess(&KApcState);                 //脱离进程(恢复CR3)
    }
    __except (1)
    {
        if (bAttachFlag)
        {
            if (pMap != NULL)
            {
                MmUnmapIoSpace(pMap, nOutBufSize);
            }
            KeUnstackDetachProcess(&KApcState);
        }
        KdPrint(("[test] %s: __except\r\n", __FUNCTION__));
        status = STATUS_UNSUCCESSFUL;       //返回失败
    }

    ObDereferenceObject(pEprocess);     //PsLookupProcessByProcessId函数会增加引用计数, 所以在使用完成后需要减计数
    return status;
}

NTSTATUS WriteProcMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    pOutBuf;
    PAGED_CODE();
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hHandle = ((TARGET_PROC*)pInBuf)->m_hHandle;
    PVOID pSrcAddr = ((TARGET_PROC*)pInBuf)->m_TargetAddres;
    unsigned __int64 nData = ((TARGET_PROC*)pInBuf)->m_nData;
    //ULONG uCr3 = 0;
    //ULONG nRetBytes = 0;

    if (nInSize < sizeof(TARGET_PROC))
    {
        KdPrint(("[test] %s Err, nInSize < sizeof(TARGET_PROC)\n", __FUNCTION__));
        return -1;
    }

    //status = GetCr3(&hHandle, sizeof(hHandle), &uCr3, sizeof(uCr3), &nRetBytes);
    //if (!NT_SUCCESS(status))
    //{
    //    KdPrint(("[test] %s: GetCr3 Err, status=%08X\r\n", __FUNCTION__, status));
    //}
    //__try 
    //{
    //    __asm cli;
    //    __writecr0(__readcr0() & ~0x10000);  //WP = 0
    //    ULONG OldCr3 = __readcr3();
    //    __writecr3((ULONG)uCr3);
    //    RtlCopyMemory(pSrcAddr, pOutBuf, nOutBufSize);
    //    __writecr3(OldCr3);
    //    __writecr0(__readcr0() | 0x10000);  //WP = 1
    //    __asm sti;
    //}
    //__except (1) 
    //{
    //    KdPrint(("[test] %s __except\n", __FUNCTION__));
    //    __asm sti;
    //}
    PEPROCESS pEprocess = NULL;
    KAPC_STATE KApcState = { 0 };
    BOOLEAN bAttachFlag = FALSE;
    PHYSICAL_ADDRESS PhysicalAddr = { 0 };
    PVOID pMap = NULL;
    status = PsLookupProcessByProcessId(hHandle, &pEprocess);       //获取pid;
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: PsLookupProcessByProcessId Err, status=%08X\r\n", __FUNCTION__, status));
        return status;
    }
    __try
    {
        KeStackAttachProcess(pEprocess, &KApcState);        //挂靠进程(切换CR3)
        bAttachFlag = TRUE;
        PhysicalAddr = MmGetPhysicalAddress(pSrcAddr);
        pMap = MmMapIoSpace(PhysicalAddr, nOutBufSize, MmNonCached);    //将要读取的地址重新映射一份线性地址
        if (pMap != NULL)
        {
            KdPrint(("[test] %s: nData=%p &nData=%p\r\n", __FUNCTION__, nData, &nData));
            RtlCopyMemory(pMap, &nData, nOutBufSize);          //写数据
            *pNumberOfBytes = nOutBufSize;
            MmUnmapIoSpace(pMap, nOutBufSize);
            status = STATUS_SUCCESS;
        }
        else
        {
            KdPrint(("[test] %s: MmMapIoSpace Err, pSrcAddr=%p, PhysicalAddr=%p\r\n", __FUNCTION__, pSrcAddr, PhysicalAddr));
            status = STATUS_UNSUCCESSFUL;
        }
        KeUnstackDetachProcess(&KApcState);                 //脱离进程(恢复CR3)
    }
    __except (1)
    {
        if (bAttachFlag)
        {
            if (pMap != NULL)
            {
                MmUnmapIoSpace(pMap, nOutBufSize);
            }
            KeUnstackDetachProcess(&KApcState);
        }
        KdPrint(("[test] %s: __except\r\n", __FUNCTION__));
        status = STATUS_UNSUCCESSFUL;       //返回失败
    }

    ObDereferenceObject(pEprocess);     //PsLookupProcessByProcessId函数会增加引用计数, 所以在使用完成后需要减计数
    return status;
}

ULONG GetSystemMajorVersion()
{
    OSVERSIONINFOW info = { 0 };
    info.dwOSVersionInfoSize = sizeof(info);

    NTSTATUS status = RtlGetVersion(&info);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: RtlGetVersion Err, status=%08X\r\n", __FUNCTION__, status));
        return 0;
    }
    return info.dwMajorVersion;
}

NTSTATUS EnumMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(pInBuf);
    UNREFERENCED_PARAMETER(nInSize);
    UNREFERENCED_PARAMETER(pOutBuf);
    UNREFERENCED_PARAMETER(nOutBufSize);
    UNREFERENCED_PARAMETER(pNumberOfBytes);
    //NTSTATUS status = STATUS_SUCCESS;
    //HANDLE hHandle = *(HANDLE*)pInBuf;
    //PEPROCESS pEprocess = NULL;
    //ULONG uMajorVersion = 0;
    //PVOID pVADRoot = 0;
    //MEM_INFO* pProcessBuffer = (MEM_INFO*)pOutBuf;
    //ULONG uNodeCnt = 0;
    //ULONG TreeLevel = (ULONG)-1;

    //if (nInSize < sizeof(HANDLE))
    //{
    //    KdPrint(("[test] %s: nInSize < sizeof(HANDLE)\r\n", __FUNCTION__));
    //    return -1;
    //}

    //status = PsLookupProcessByProcessId(hHandle, &pEprocess);       //获取pid;
    //if (!NT_SUCCESS(status))
    //{
    //    KdPrint(("[test] %s: PsLookupProcessByProcessId Err, status=%08X\r\n", __FUNCTION__, status));
    //    return -1;
    //}
    //uMajorVersion = GetSystemMajorVersion();
    //if (uMajorVersion == 5)
    //{
    //    pVADRoot = (PVOID)((ULONG)pEprocess + 0x11c);
    //}
    //else if (uMajorVersion == 6)
    //{
    //    pVADRoot = (PVOID)((ULONG)pEprocess + 0x278);
    //}

    //KdPrint(("[test] %s: eprocess:%p, pVADRoot=%08X\r\n", __FUNCTION__, pEprocess, pVADRoot));
    //if (nOutBufSize == 0)
    //{
    //    EnumVadTree(pVADRoot, &uNodeCnt, TreeLevel, NULL);
    //}
    //else
    //{
    //    EnumVadTree(pVADRoot, &uNodeCnt, TreeLevel, pProcessBuffer);
    //}

    //*pNumberOfBytes = uNodeCnt * sizeof(MEM_INFO);
    //KdPrint(("[test] %s: pNumberOfBytes=%d\r\n", __FUNCTION__, *pNumberOfBytes));
    return status;
}


VOID EnumVadTree(PVOID pNode, PULONG puNodeCnt, ULONG uLastTreeLevel, MEM_INFO* pOutBuf)
{
    UNREFERENCED_PARAMETER(pNode);
    UNREFERENCED_PARAMETER(puNodeCnt);
    UNREFERENCED_PARAMETER(pOutBuf);
    UNREFERENCED_PARAMETER(uLastTreeLevel);
    //ULONG uCurLevel = uLastTreeLevel + 1;
    //PVOID pLeftChild = *(PVOID*)((ULONG)pNode + 4);
    //PVOID pRightChild = *(PVOID*)((ULONG)pNode + 8);
    //PVOID pSubsection = *(PVOID*)((ULONG)pNode + 0x24);
    //PVOID pControlArea = NULL;
    //PFILE_OBJECT pFileObj = NULL;
    //pControlArea; pFileObj;

    //if (pLeftChild != NULL)
    //{
    //    EnumVadTree(pLeftChild, puNodeCnt, uCurLevel, pOutBuf);
    //}
    ////输出缓冲区写入内容, 节点计数增加
    //if (pOutBuf != NULL)
    //{
    //    ULONG nIdx = *puNodeCnt;
    //    pOutBuf[nIdx].m_NodeAddr = (ULONG)pNode;
    //    pOutBuf[nIdx].m_Level = uCurLevel;
    //    pOutBuf[nIdx].m_StartAddr = *(ULONG*)((ULONG)pNode + 0xC);
    //    pOutBuf[nIdx].m_EndAddr = *(ULONG*)((ULONG)pNode + 0x10);
    //    pOutBuf[nIdx].m_Flag1.u = *(ULONG*)((ULONG)pNode + 0x14);
    //    pOutBuf[nIdx].m_Flag2 = *(ULONG*)((ULONG)pNode + 0x20);

    //    ULONG uPrivateFlag = pOutBuf[nIdx].m_Flag1.u;
    //    ULONG uTmp = uPrivateFlag;
    //    uTmp;
    //    uPrivateFlag = uPrivateFlag & 0x300000;
    //    if (uPrivateFlag != 0)
    //    {
    //        //DbgBreakPoint();
    //        pControlArea = *(PVOID*)pSubsection;
    //        pFileObj = (PVOID)((ULONG)pControlArea + 0x24);
    //        pFileObj = *(PVOID*)pFileObj;
    //        pFileObj = (PFILE_OBJECT)((ULONG)pFileObj & 0xFFFFFFFC);
    //        if (pFileObj->FileName.Buffer != NULL)
    //        {
    //            RtlCopyMemory(pOutBuf[nIdx].m_FilePath, pFileObj->FileName.Buffer, pFileObj->FileName.Length);

    //        }
    //        //KdPrint(("Cnt=%d, Level=%d, start:%08X, end:%08X, %ws\r\n",
    //        //    (*puNodeCnt), uCurLevel, pOutBuf[*puNodeCnt].m_StartAddr, pOutBuf[*puNodeCnt].m_EndAddr, pFileObj->FileName.Buffer));
    //    }
    //    //KdPrint(("Cnt=%d, Level=%d, start:%08X, end:%08X\r\n", (*puNodeCnt), uCurLevel, pOutBuf[*puNodeCnt].m_StartAddr, pOutBuf[*puNodeCnt].m_EndAddr));
    //}

    //(*puNodeCnt)++;
    //if (pRightChild != NULL)
    //{
    //    EnumVadTree(pRightChild, puNodeCnt, uCurLevel, pOutBuf);
    //}

    return;
}

NTSTATUS GetSysModule(PDRIVER_OBJECT DriverObject, PVOID pInBuf, ULONG nInSize, DRIVER_INFO* pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytesRead)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(pInBuf);
    UNREFERENCED_PARAMETER(nInSize);
    UNREFERENCED_PARAMETER(pOutBuf);
    UNREFERENCED_PARAMETER(nOutBufSize);
    UNREFERENCED_PARAMETER(pNumberOfBytesRead);
    //NTSTATUS status = STATUS_SUCCESS;
    //PVOID pLdrDataTabEntry = DriverObject->DriverSection;
    //PLIST_ENTRY pList = (PLIST_ENTRY)pLdrDataTabEntry;
    //pList = pList->Flink;
    //PLIST_ENTRY pNext = pList;

    //PULONG pDllBase = NULL;
    //PULONG pDllSize = NULL;
    //ULONG targetAddr = 0;
    //PUNICODE_STRING pFullName = NULL;
    //PUNICODE_STRING pBaseName = NULL;

    //ULONG nWriteSize = 0;
    //DRIVER_INFO* pDivInfo = pOutBuf;

    //if (nInSize < sizeof(PVOID))
    //{
    //    KdPrint(("[test] %s: nInSize < sizeof(HANDLE)\r\n", __FUNCTION__));
    //    return STATUS_UNSUCCESSFUL;
    //}

    //if (nOutBufSize < sizeof(DRIVER_INFO))
    //{
    //    KdPrint(("[test] %s: nOutBufSize < sizeof(DRIVER_INFO)\r\n", __FUNCTION__));
    //    return STATUS_UNSUCCESSFUL;
    //}

    //targetAddr = *(PULONG)pInBuf;

    //do
    //{
    //    pLdrDataTabEntry = pNext;
    //    pDllBase = (PULONG)((ULONG)pLdrDataTabEntry + 0x18);
    //    pDllSize = (PULONG)((ULONG)pLdrDataTabEntry + 0x20);
    //    pFullName = (PUNICODE_STRING)((ULONG)pLdrDataTabEntry + 0x24);
    //    pBaseName = (PUNICODE_STRING)((ULONG)pLdrDataTabEntry + 0x2c);

    //    if (*pDllBase != 0)
    //    {
    //        //KdPrint(("[test] DivName:%ws, DivBase:%p, DivSize:%d, Path:%ws\r\n",
    //        //    pBaseName->Buffer, *pDllBase, *pDllSize, pFullName->Buffer));
    //        ULONG nBase = *pDllBase;
    //        ULONG nSize = *pDllSize;
    //        if (targetAddr >= nBase && targetAddr < nBase + nSize)
    //        {
    //            pDivInfo->m_nBase = nBase;
    //            pDivInfo->m_nSize = nSize;
    //            RtlCopyMemory(pDivInfo->m_szName, pBaseName->Buffer, pBaseName->Length + 2);
    //            RtlCopyMemory(pDivInfo->m_szPatch, pFullName->Buffer, pFullName->Length + 2);
    //            nWriteSize = sizeof(DRIVER_INFO);
    //            break;
    //        }
    //    }
    //    pNext = pNext->Flink;
    //} while (pNext != pList);

    //*pNumberOfBytesRead = nWriteSize;
    return status;
}

NTSTATUS ReadMemory(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    if (nOutBufSize == 0 || nInSize < sizeof(PVOID))
    {
        KdPrint(("[test] %s: Err, nOutBufSize=%d, nInSize=%d\r\n", __FUNCTION__, nOutBufSize, nInSize));
        return STATUS_UNSUCCESSFUL;
    }

    if (!MmIsAddressValid(pInBuf) || !MmIsAddressValid(pOutBuf) || !MmIsAddressValid(pNumberOfBytes))
    {
        KdPrint(("[test] %s: Invalid Memroy!, pInBuf=%p, pOutBuf=%p, pNumberOfBytes=%p\r\n",
            __FUNCTION__, pInBuf, pOutBuf, pNumberOfBytes));
        return STATUS_UNSUCCESSFUL;
    }

    PVOID pDstAddr = *(PVOID*)pInBuf;
    if (!MmIsAddressValid(pDstAddr))
    {
        KdPrint(("[test] %s: Invalid Memroy!, pDstAddr=%p\r\n", __FUNCTION__, pDstAddr));
        return STATUS_UNSUCCESSFUL;
    }
    __try
    {
        RtlCopyMemory(pOutBuf, pDstAddr, nOutBufSize);
        *pNumberOfBytes = nOutBufSize;
    }
    __except (1)
    {
        KdPrint(("[test] %s: __except, pOutBuf=%P, pDstAddr=%p, ReadSize=%d\r\n", __FUNCTION__, pOutBuf, pDstAddr, nOutBufSize));
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}

NTSTATUS ReadFileData(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    HANDLE hFile = NULL;
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };
    IO_STATUS_BLOCK IoStatusBlock = { 0 };
    FILE_STANDARD_INFORMATION fsi = { 0 };
    UNICODE_STRING ustrPath = { 0 };


    if (nInSize <= sizeof(WCHAR))
    {
        KdPrint(("[test] %s: nInSize<=sizeof(WCHAR), nInSize=%d\r\n", __FUNCTION__, nInSize));
        return STATUS_UNSUCCESSFUL;
    }
    RtlInitUnicodeString(&ustrPath, pInBuf);
    KdPrint(("[test] %s: path=%ws\r\n", __FUNCTION__, pInBuf));


    //初始化对象属性
    InitializeObjectAttributes(&ObjAttributes,
        &ustrPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    //打开文件
    status = ZwCreateFile(&hFile,   //句柄                       
        SYNCHRONIZE | FILE_ANY_ACCESS,        //访问权限
        &ObjAttributes,             //对象属性  
        &IoStatusBlock,             //返回状态信息
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,     //共享访问属性
        FILE_OPEN,                  //指定文件不存在时做的操作
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,   //打开方式
        NULL, 0);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwCreateFile Error, status=%p\r\n", __FUNCTION__, status));
        return status;
    }
    else
    {
        KdPrint(("[test] %s: ZwCreateFile yes, status=%p\r\n", __FUNCTION__, status));
    }

    //获取文件信息
    status = ZwQueryInformationFile(hFile, &IoStatusBlock, &fsi, sizeof(fsi), FileStandardInformation);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwQueryInformationFile Error, status=%p\r\n", __FUNCTION__, status));
        ZwClose(hFile);
        return status;
    }

    //如果输出缓冲区大小为0，则返回文件大小
    if (nOutBufSize == 0)
    {
        *pNumberOfBytes = fsi.AllocationSize.LowPart;
        ZwClose(hFile);
        return status;
    }

    //如果输出缓冲区大小小于文件大小，返回失败
    if (nOutBufSize < fsi.AllocationSize.LowPart)
    {
        KdPrint(("[test] %s: nOutBufSize < FileSize, nOutBufSize=%d\r\n", __FUNCTION__, nOutBufSize));
        ZwClose(hFile);
        return STATUS_UNSUCCESSFUL;
    }

    //读数据
    try
    {
        RtlZeroMemory(pOutBuf, fsi.AllocationSize.LowPart);
        status = ZwReadFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pOutBuf, fsi.AllocationSize.LowPart, 0, NULL);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("[test] %s: ZwReadFile Error, status=%p\r\n", __FUNCTION__, status));
            ZwClose(hFile);
            return status;
        }
    }
    __except (1)
    {
        KdPrint(("[test] %s: __except\r\n", __FUNCTION__));
    }

    *pNumberOfBytes = fsi.AllocationSize.LowPart;
    ZwClose(hFile);
    return status;
}

NTSTATUS UnLockFile(PVOID pInBuf, ULONG nInSize)
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    PSYSTEM_HANDLE_INFORMATION Handles = NULL;
    PSYSTEM_HANDLE_TABLE_ENTRY_INFO HandleInfo = NULL;
    POBJECT_NAME_INFORMATION NameInfo = NULL;
    PVOID Buffer = NULL;
    ULONG BufferSize = 4096;
    ULONG ReturnLength = 0;;
    ULONG i;

    if (nInSize <= sizeof(WCHAR))
    {
        KdPrint(("[test] %s: nInSize <= sizeof(WCHAR)!\r\n", __FUNCTION__));
        return Status;
    }

    //KdPrint(("[test] %s: Path=%ws\r\n", __FUNCTION__, pInBuf));
    //KdBreakPoint();

    //申请缓冲区用于存放名称字符串, 这里为了省事直接申请了一个页
    NameInfo = (POBJECT_NAME_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, BufferSize, '1234');
    if (!NameInfo) {
        return STATUS_NO_MEMORY;
    }
    //获取句柄信息
UnLockFile_Retry:
    Buffer = ExAllocatePoolWithTag(NonPagedPool, BufferSize, '1234');		// 申请空间
    if (!Buffer) {
        return STATUS_NO_MEMORY;
    }
    Status = ZwQuerySystemInformation(SystemHandleInformation,	//获取信息
        Buffer,
        BufferSize,
        &ReturnLength
    );
    if (Status == STATUS_INFO_LENGTH_MISMATCH) {		//如果BufSize不够大, 则释放先前申请到的缓冲区重新申请一个
        ExFreePool(Buffer);
        BufferSize = ReturnLength;
        goto UnLockFile_Retry;
    }

    if (NT_SUCCESS(Status)) 
    {
        Handles = (PSYSTEM_HANDLE_INFORMATION)Buffer;
        for (i = 0, HandleInfo = &(Handles->Handles[0]);
            i < Handles->NumberOfHandles;
            i++, HandleInfo++) 
        {
            Status = ObReferenceObjectByPointer(	//试着通过句柄去获取文件对象
                HandleInfo->Object,
                GENERIC_ALL,
                *IoFileObjectType,
                KernelMode);
            if (NT_SUCCESS(Status))     //如果函数成功说明该句柄是个文件对象的句柄
            {		
                Status = ObQueryNameString(HandleInfo->Object, NameInfo, 4096, &ReturnLength);		//获取文件名
                if (NT_SUCCESS(Status)) 
                {
                    if (wcsstr(NameInfo->Name.Buffer, (PWCHAR)pInBuf) != NULL)  //判断是不是目标文件
                    {		
                        KdPrint(("[test] %s: Path=%wZ\r\n", __FUNCTION__, NameInfo->Name));
                        PEPROCESS Process = NULL;
                        Status = PsLookupProcessByProcessId((HANDLE)HandleInfo->UniqueProcessId, &Process);	//获取进程对象
                        if (NT_SUCCESS(Status)) 
                        {
                            KAPC_STATE ApcState;
                            KeStackAttachProcess(Process, &ApcState);		//附加到目标进程

                            //关闭句柄
                            ZwClose((HANDLE)HandleInfo->HandleValue);		//在目标进程中调用ZwClose关闭句柄
                            DbgPrint("[51asm] ZwClose HandleValue:%d\n", HandleInfo->HandleValue);

                            KeUnstackDetachProcess(&ApcState);		//恢复进程
                            ObDereferenceObject(Process);		//释放PsLookupProcessByProcessId得到的进程对象
                        }

                    }
                }
                ObDereferenceObject(HandleInfo->Object);	//释放ObReferenceObjectByPointer得到的文件对象
            }
        }
    }

    //资源清理
    ExFreePool(Buffer);
    ExFreePool(NameInfo);
    return STATUS_SUCCESS;
    
}

NTSTATUS FD_SetFileCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
)
{
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->UserIosb->Status = Irp->IoStatus.Status;
    Irp->UserIosb->Information = Irp->IoStatus.Information;

    KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);

    IoFreeIrp(Irp);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

HANDLE  FD_OpenFile(WCHAR szFileName[])
{
    NTSTATUS            ntStatus;
    UNICODE_STRING      FileName;
    OBJECT_ATTRIBUTES   objectAttributes;
    HANDLE              hFile;
    IO_STATUS_BLOCK     ioStatus;

    // 确保IRQL在PASSIVE_LEVEL上 
    if (KeGetCurrentIrql() > PASSIVE_LEVEL)
        return NULL;

    // 初始化文件名 
    RtlInitUnicodeString(&FileName, szFileName);

    //初始化对象属性 
    InitializeObjectAttributes(&objectAttributes, &FileName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

    // 打开文件 
    ntStatus = IoCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objectAttributes, &ioStatus, \
        0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_DELETE, FILE_OPEN, 0, NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
    if (!NT_SUCCESS(ntStatus))
        return NULL;

    return  hFile;
}

BOOLEAN FD_StripFileAttributes(HANDLE FileHandle)
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PFILE_OBJECT            fileObject;
    PDEVICE_OBJECT          DeviceObject;
    PIRP                    Irp;
    KEVENT                  SycEvent;
    FILE_BASIC_INFORMATION  FileInformation;
    IO_STATUS_BLOCK         ioStatus;
    PIO_STACK_LOCATION      irpSp;

    // 获取文件对象 
    ntStatus = ObReferenceObjectByHandle(FileHandle, DELETE,
        *IoFileObjectType, KernelMode, (PVOID*)&fileObject, NULL);
    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("[test] %s:ObReferenceObjectByHandle error!, status=%p\r\n", __FUNCTION__, ntStatus));
        return FALSE;
    }

    // 获取与指定文件对象相关联的设备对象 
    DeviceObject = IoGetRelatedDeviceObject(fileObject);

    // 创建IRP 
    Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);
    if (Irp == NULL)
    {
        ObDereferenceObject(fileObject);

        KdPrint(("[test] %s:IoAllocateIrp error!\r\n", __FUNCTION__));
        return FALSE;
    }

    // 初始化同步事件对象 
    KeInitializeEvent(&SycEvent, SynchronizationEvent, FALSE);

    memset(&FileInformation, 0, 0x28);
    FileInformation.FileAttributes = FILE_ATTRIBUTE_NORMAL;

    // 初始化IRP 
    Irp->AssociatedIrp.SystemBuffer = &FileInformation;
    Irp->UserEvent = &SycEvent;
    Irp->UserIosb = &ioStatus;
    Irp->Tail.Overlay.OriginalFileObject = fileObject;
    Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
    Irp->RequestorMode = KernelMode;

    // 设置IRP堆栈信息 
    irpSp = IoGetNextIrpStackLocation(Irp);
    irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
    irpSp->DeviceObject = DeviceObject;
    irpSp->FileObject = fileObject;
    irpSp->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
    irpSp->Parameters.SetFile.FileInformationClass = FileBasicInformation;
    irpSp->Parameters.SetFile.FileObject = fileObject;

    // 设置完成例程 
    IoSetCompletionRoutine(Irp, FD_SetFileCompletion, NULL, TRUE, TRUE, TRUE);

    // 派发IRP 
    IoCallDriver(DeviceObject, Irp);

    // 等待IRP的完成 
    KeWaitForSingleObject(&SycEvent, Executive, KernelMode, TRUE, NULL);

    // 递减引用计数 
    ObDereferenceObject(fileObject);

    return TRUE;
}

BOOLEAN FD_DeleteFile(HANDLE FileHandle)
{
    NTSTATUS          ntStatus = STATUS_SUCCESS;
    PFILE_OBJECT      fileObject;
    PDEVICE_OBJECT    DeviceObject;
    PIRP              Irp;
    KEVENT            SycEvent;
    FILE_DISPOSITION_INFORMATION    FileInformation;
    IO_STATUS_BLOCK                 ioStatus;
    PIO_STACK_LOCATION              irpSp;
    PSECTION_OBJECT_POINTERS        pSectionObjectPointer;

    // 获取文件对象 
    ntStatus = ObReferenceObjectByHandle(FileHandle, DELETE,
        *IoFileObjectType, KernelMode, (PVOID*)&fileObject, NULL);
    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("[test] %s:ObReferenceObjectByHandle error!, status=%p\r\n", __FUNCTION__, ntStatus));
        return FALSE;
    }

    // 获取与指定文件对象相关联的设备对象 
    DeviceObject = IoGetRelatedDeviceObject(fileObject);

    // 创建IRP 
    Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);
    if (Irp == NULL)
    {
        ObDereferenceObject(fileObject);
        KdPrint(("[test] %s:FD_DeleteFile IoAllocateIrp error\r\n", __FUNCTION__));
        return FALSE;
    }

    // 初始化同步事件对象 
    KeInitializeEvent(&SycEvent, SynchronizationEvent, FALSE);

    FileInformation.DeleteFile = TRUE;

    // 初始化IRP 
    Irp->AssociatedIrp.SystemBuffer = &FileInformation;
    Irp->UserEvent = &SycEvent;
    Irp->UserIosb = &ioStatus;
    Irp->Tail.Overlay.OriginalFileObject = fileObject;
    Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
    Irp->RequestorMode = KernelMode;

    // 设置IRP堆栈 
    irpSp = IoGetNextIrpStackLocation(Irp);
    irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
    irpSp->DeviceObject = DeviceObject;
    irpSp->FileObject = fileObject;
    irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
    irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
    irpSp->Parameters.SetFile.FileObject = fileObject;

    // 设置完成例程 
    IoSetCompletionRoutine(Irp, FD_SetFileCompletion, NULL, TRUE, TRUE, TRUE);

    // 如果没有这3行，就无法删除正在运行的文件 
    pSectionObjectPointer = fileObject->SectionObjectPointer;
    pSectionObjectPointer->ImageSectionObject = 0;
    pSectionObjectPointer->DataSectionObject = 0;

    // 派发IRP 
    IoCallDriver(DeviceObject, Irp);

    // 等待IRP完成 
    KeWaitForSingleObject(&SycEvent, Executive, KernelMode, TRUE, NULL);

    // 递减引用计数 
    ObDereferenceObject(fileObject);

    return TRUE;
}

BOOLEAN ForceDeleteFile(WCHAR szFileName[])
{
    HANDLE      hFile = NULL;
    BOOLEAN     status = FALSE;

    __try {
        // 打开文件 
        if ((hFile = FD_OpenFile(szFileName)) == NULL)
        {
            KdPrint(("[test] %s:FD_OpenFile error!\r\n",__FUNCTION__));
            return FALSE;
        }

        // //去掉只读属性，才能删除只读文件 
        if (FD_StripFileAttributes(hFile) == FALSE)
        {
            ZwClose(hFile);
            KdPrint(("[test] %s:FD_StripFileAttributes error!\r\n", __FUNCTION__));
            return FALSE;
        }

        // 删除文件 
        status = FD_DeleteFile(hFile);
        ZwClose(hFile);

        return status;

    }
    __except (1) {
        KdPrint(("[test] %s:exception!\r\n", __FUNCTION__));
    }

    return FALSE;
}

NTSTATUS SmashFile(PVOID pInBuf, ULONG nInSize)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    if (nInSize <= sizeof(WCHAR))
    {
        KdPrint(("[test] %s: nInSize <= sizeof(WCHAR)!\r\n", __FUNCTION__));
        return status;
    }

    if (ForceDeleteFile((WCHAR*)pInBuf))
    {
        KdPrint(("[test] %s: SmashFile OK!\r\n", __FUNCTION__));
        status = STATUS_SUCCESS;
    }
    else
    {
        KdPrint(("[test] %s: SmashFile Error!\r\n", __FUNCTION__));
    }
    return status;
}

//遍历进程中的线程
NTSTATUS EnumThread(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    THREAD_INFO* pThreadInfo = pOutBuf;
    ULONG uCurReadSize = 0;
    HANDLE hPid = NULL;
    PETHREAD pETHREAD = NULL;
    PSYSTEM_PROCESSES pProcessInfo = NULL;
    PVOID pProcessBuffer = NULL;
    ULONG uBufSize = 0x1000;
    ULONG uRetBytes = 0;
    HANDLE hThread = NULL;
    OBJECT_ATTRIBUTES ObjAttributes = { 0 };

    if (nInSize < sizeof(HANDLE))
    {
        KdPrint(("[test] %s: nInSize < sizeof(HANDLE)\r\n", __FUNCTION__));
        return status;
    }
    hPid = *(PHANDLE)pInBuf;

EnumThread_Retry:
    pProcessBuffer = ExAllocatePoolWithTag(NonPagedPool, uBufSize, '1234');		// 申请空间
    if (!pProcessBuffer) {
        KdPrint(("[test] %s: STATUS_NO_MEMORY\r\n", __FUNCTION__));
        return STATUS_NO_MEMORY;
    }
    RtlZeroMemory(pProcessBuffer, uBufSize);
    status = ZwQuerySystemInformation(SystemProcessInformation,	//获取信息
        pProcessBuffer,
        uBufSize,
        &uRetBytes
    );
    if (status == STATUS_INFO_LENGTH_MISMATCH) {		//如果BufSize不够大, 则释放先前申请到的缓冲区重新申请一个
        ExFreePool(pProcessBuffer);
        uBufSize = uRetBytes;
        goto EnumThread_Retry;
    }

    if (!NT_SUCCESS(status))
    {
        KdPrint(("[test] %s: ZwQuerySystemInformation Error, status=%p\r\n", __FUNCTION__,status));
        goto EnumThread_Retry;
    }

    pProcessInfo = pProcessBuffer;
    //DbgBreakPoint();
    while (TRUE)
    {
        if (pProcessInfo->UniqueProcessId == hPid)
        {
            if (nOutBufSize < pProcessInfo->NumberOfThreads * sizeof(THREAD_INFO))
            {
                //如果没给输出缓冲区, 则返回缓冲区所需大小
                uCurReadSize = pProcessInfo->NumberOfThreads * sizeof(THREAD_INFO);
                status = STATUS_SUCCESS;
                goto ENUMTHREAD_EXIT;
            }
            for (ULONG i = 0; i < pProcessInfo->NumberOfThreads; i++)
            {
                //获取线程句柄
                InitializeObjectAttributes(&ObjAttributes,
                    NULL,
                    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                    NULL, NULL);
                status = ZwOpenThread(&hThread, GENERIC_ALL, &ObjAttributes, &pProcessInfo->Threads[i].ClientId);
                if (!NT_SUCCESS(status))
                {
                    KdPrint(("[test] %s: ZwOpenThread Error, status=%p\r\n", __FUNCTION__, status));
                    goto ENUMTHREAD_EXIT;
                }
                PsLookupThreadByThreadId(pProcessInfo->Threads[i].ClientId.UniqueThread, &pETHREAD);
                pThreadInfo[i].m_uCid = pProcessInfo->Threads[i].ClientId.UniqueThread;
                pThreadInfo[i].m_uPriority = pProcessInfo->Threads[i].Priority;
                pThreadInfo[i].m_uState = pProcessInfo->Threads[i].ThreadState;
                pThreadInfo[i].m_uSwapCnt = pProcessInfo->Threads[i].ContextSwitches;
                pThreadInfo[i].m_pETHREAD = pETHREAD;
                pThreadInfo[i].m_TEB = PsGetThreadTeb(pETHREAD);
                ZwQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &pThreadInfo[i].m_pStartAddr, sizeof(PVOID), &uRetBytes);
                ObDereferenceObject(pETHREAD);
                pETHREAD = NULL;
                ZwClose(hThread);
                hThread = NULL;
                uCurReadSize += sizeof(THREAD_INFO);
            }
        }
        if (pProcessInfo->NextEntryOffset == 0)
        {
            break;
        }
        pProcessInfo = (PSYSTEM_PROCESSES)((PCHAR)pProcessInfo + pProcessInfo->NextEntryOffset);
    }
ENUMTHREAD_EXIT:  
    ExFreePool(pProcessBuffer);
    *pNumberOfBytes = uCurReadSize;
    KdPrint(("[test] %s: status=%p\r\n", __FUNCTION__, status));
    return status;
}

NTSTATUS TerminateProcess(PVOID pInBuf, ULONG nInSize)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    HANDLE hPid = NULL;
    PspTerminateThreadByPointer pfnTerminateThreadByPointer = NULL;
    PETHREAD pThread = NULL;
    PEPROCESS pProcess = NULL;


    //检查输入参数
    if (nInSize < sizeof(PROCESS_EXIT))
    {
        KdPrint(("[test] %s: nInSize < sizeof(PROCESS_EXIT)\r\n", __FUNCTION__));
        return status;
    }
    hPid = ((PROCESS_EXIT*)pInBuf)->m_hPid;
    pfnTerminateThreadByPointer = (PspTerminateThreadByPointer)((PROCESS_EXIT*)pInBuf)->m_pfnPspTerminateThreadByPointer;
    
    //遍历系统内所有句柄寻找线程
    for (ULONG i = 0; i < 0x10000; i++)
    {
        status = PsLookupThreadByThreadId((HANDLE)i, &pThread);
        if (NT_SUCCESS(status))
        {
            pProcess = IoThreadToProcess(pThread);
            if (PsGetProcessId(pProcess) == hPid)
            {
                //如果线程属于目标进程则结束
                status = pfnTerminateThreadByPointer(pThread, 0, FALSE);
            }
            ObDereferenceObject(pThread);
            pThread = NULL;
            
            //判断下TerminateThreadByPointer的执行结果
            if (!NT_SUCCESS(status))
            {
                KdPrint(("[test] %s: TerminateThreadByPointer Error, status=%p\r\n", __FUNCTION__, status));
                return status;
            }
        }
    }
    status = STATUS_SUCCESS;
    return status;
}

NTSTATUS EnumSysCallback(PVOID pInBuf, ULONG nInSize, PVOID pOutBuf, ULONG nOutBufSize, PULONG pNumberOfBytes)
{
    NTSTATUS status = STATUS_SUCCESS;
    SYSTEM_CALLBACK* pSysCallback = (SYSTEM_CALLBACK*)pInBuf;
    PEX_CALLBACK pCallBack = NULL;
    ULONG uCount = 0;
    PEX_CALLBACK_ROUTINE_BLOCK pCallbackRoutineBlock = NULL;
    ULONG uWriteSize = 0;
    //检查参数
    if (nInSize < sizeof(SYSTEM_CALLBACK))
    {
        KdPrint(("[test] %s: nInSize < sizeof(SYSTEM_CALLBACK)\r\n", __FUNCTION__));
        return status;
    }
    if (nOutBufSize < pSysCallback->m_uCount * sizeof(PVOID))
    {
        KdPrint(("[test] %s: nOutBufSize < pSysCallback->m_uCount * sizeof(PVOID)\r\n", __FUNCTION__));
        return status;
    }

    KdPrint(("[test] %s: Count=%d, m_pNotifyRoutine=%p\r\n", __FUNCTION__, pSysCallback->m_uCount, pSysCallback->m_pNotifyRoutine));
    //遍历
    __try
    {
        pCallBack = (PEX_CALLBACK)pSysCallback->m_pNotifyRoutine;
        uCount = pSysCallback->m_uCount;
        for (ULONG i = 0; i < uCount; i++)
        {
            if (pCallBack[i].RoutineBlock.Object != NULL)
            {
                pCallbackRoutineBlock = (PEX_CALLBACK_ROUTINE_BLOCK)(pCallBack[i].RoutineBlock.Value & ~MAX_FAST_REFS);
                pCallbackRoutineBlock->Function;
                RtlCopyMemory((PCHAR)pOutBuf + uWriteSize, &pCallbackRoutineBlock->Function, sizeof(PVOID));
                uWriteSize += sizeof(PVOID);
                if (uWriteSize + sizeof(PVOID) > nOutBufSize)
                {
                    break;
                }
            }
        }
    }
    __except (1)
    {
        status = STATUS_UNSUCCESSFUL;
        KdPrint(("[test] %s: __except, 内存写入异常\r\n", __FUNCTION__));
    }

    *pNumberOfBytes = uWriteSize;
    return status;
}

//测试
NTSTATUS Test()
{
    NTSTATUS status = STATUS_SUCCESS;
    SYSTEM_CALLBACK sc = { 0 };
    PVOID pAry[64] = { 0 };
    ULONG dwRetByts = 0;
    sc.m_uCount = 64;
    sc.m_pNotifyRoutine = (PVOID)0xfffff8000406b500;
    status = EnumSysCallback(&sc, sizeof(sc), pAry, sizeof(pAry), &dwRetByts);
    if (NT_SUCCESS(status))
    {
        for (ULONG i = 0; i < 64; i++)
        {
            if (pAry[i] != 0)
            {
                KdPrint(("[test] %s: CreateProcess Callback=%p\r\n", __FUNCTION__, pAry[i]));
            }
        }
    }
    else
    {
        KdPrint(("[test] %s: EnumSysCallback Error, status=%p\r\n", __FUNCTION__, status));
    }
//TEST_EXIT:
    return status;
}
