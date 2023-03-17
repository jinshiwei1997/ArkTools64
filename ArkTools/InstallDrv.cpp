#include "pch.h"

#include "global.h"


//��������
BOOL scmInstallDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName,
    _In_opt_ LPCTSTR DrvPath
)
{
    SC_HANDLE  schService;

    schService = CreateService(SchSCManager, // SCManager database
        DriverName,           // name of service
        DriverName,           // name to display
        SERVICE_ALL_ACCESS,    // desired access
        SERVICE_KERNEL_DRIVER, // service type
        SERVICE_DEMAND_START,  // start type
        SERVICE_ERROR_NORMAL,  // error control type
        DrvPath,            // service's binary
        NULL,                  // no load ordering group
        NULL,                  // no tag identifier
        NULL,                  // no dependencies
        NULL,                  // LocalSystem account
        NULL                   // no password
    );
    if (schService == NULL) {
        OutputDebugString(ShowErrMsg() + L"\r\n");
        return FALSE;
    }

    CloseServiceHandle(schService);
    return TRUE;
}

//��װ����
BOOL scmStartDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService = OpenService(SchSCManager,
        DriverName,
        SERVICE_ALL_ACCESS
    );
    if (schService == NULL)
        return FALSE;

    ret = StartService(schService, 0, NULL)
        || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;

    CloseServiceHandle(schService);

    return ret;
}

//�����豸
BOOL scmOpenDevice(
    _In_ LPCTSTR DriverName,
    _Inout_opt_ PHANDLE lphDevice
)
{
    TCHAR    completeDeviceName[64];
    HANDLE   hDevice;

    RtlSecureZeroMemory(completeDeviceName, sizeof(completeDeviceName));
    wsprintf(completeDeviceName, TEXT("\\\\.\\%s"), DriverName);
    hDevice = CreateFile(completeDeviceName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(ShowErrMsg() + L"\r\n");
        return FALSE;
    }

    if (lphDevice) {
        *lphDevice = hDevice;
    }
    else {
        CloseHandle(hDevice);
    }

    return TRUE;
}

//ֹͣ����
BOOL scmStopDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    BOOL            ret;
    INT             iRetryCount;
    SC_HANDLE       schService;
    SERVICE_STATUS  serviceStatus;

    ret = FALSE;
    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        return ret;
    }

    iRetryCount = 5;
    do {
        SetLastError(0);

        ret = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);
        if (ret != FALSE)
            break;

        if (GetLastError() != ERROR_DEPENDENT_SERVICES_RUNNING)
            break;

        Sleep(1000);
        iRetryCount--;
    } while (iRetryCount);

    CloseServiceHandle(schService);
    return ret;
}

//ж�ط���
BOOL scmRemoveDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       bResult = FALSE;

    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService) {
        bResult = DeleteService(schService);
        CloseServiceHandle(schService);
    }
    return bResult;
}

//ֹͣ������ж��
BOOL scmUnloadDeviceDriver(
    _In_ LPCTSTR Name
)
{
    SC_HANDLE schSCManager;
    BOOL      bResult = FALSE;

    if (Name == NULL) {
        return bResult;
    }

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager) {
        scmStopDriver(schSCManager, Name);
        bResult = scmRemoveDriver(schSCManager, Name);
        CloseServiceHandle(schSCManager);
    }
    return bResult;
}