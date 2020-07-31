/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#include <windows.h>
#include <tchar.h>
#include <iostream>

#include "win_service.h"

#include "win_service_ctrl.h"

/* === Install/Uninstall service === */

int SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        std::cout << "Cannot install service with code " << GetLastError() << std::endl;
        return 1;
    }

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        std::cout << "OpenSCManager failed with code " << GetLastError() << std::endl;
        return 1;
    }

    schService = CreateService(
        schSCManager,              // SCM database
        SVCNAME,                   // name of service
        SVCNAME,                   // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START,        // start type
        SERVICE_ERROR_NORMAL,      // error control type
        szPath,                    // path to service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        NULL,                      // no dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password

    if (schService == NULL) {
        std::cout << "CreateService failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return 1;
    }
    else
        std::cout << "Service installed successfully" << std::endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int SvcUnistall() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        std::cout << "OpenSCManager failed with code " << GetLastError() << std::endl;
        return 1;
    }

    schService = OpenService(
            schSCManager,       // SCM database
            SVCNAME,            // name of service
            DELETE);            // need delete access

    if (schService == NULL) {
        std::cout << "OpenService failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (!DeleteService(schService)) {
        std::cout << "Uninstall service failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }
    else
        std::cout << "Service uninstalled successfully" << std::endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int SvcStart() {
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // servicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        std::cout << "OpenSCManager failed with code " << GetLastError() << std::endl;
        return 1;
    }

    schService = OpenService(
            schSCManager,         // SCM database
            SVCNAME,              // name of service
            SERVICE_ALL_ACCESS);  // full access

    if (schService == NULL) {
        std::cout << "OpenService failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // size needed if buffer is too small
    {
        std::cout << "QueryServiceStatusEx failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING) {
        std::cout << "Cannot start the service because it is already running" << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
                schService,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE) &ssStatus,             // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded ) )              // size needed if buffer is too small
        {
            std::cout << "QueryServiceStatusEx failed with code " << GetLastError() << std::endl;
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint) {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {
                std::cout << "Timeout waiting for service to stop" << std::endl;
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return 1;
            }
        }
    }

    if (!StartService(
            schService,  // handle to service
            0,           // number of arguments
            NULL))      // no arguments
    {
        std::cout << "StartService failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }
    else
        std::cout << "Service start pending..." << std::endl;

    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
    {
        std::cout << "QueryServiceStatusEx failed with code " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
                schService,             // handle to service
                SC_STATUS_PROCESS_INFO, // info level
                (LPBYTE) &ssStatus,             // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded ) )              // if buffer too small
        {
            std::cout << "QueryServiceStatusEx failed with code " << GetLastError() << std::endl;
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint) {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else {
            if (GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint) {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
        std::cout << "Service started successfully" << std::endl;
    }
    else {
        std::cout << "Service not started" << std::endl;
        std::cout << "  Current State: " << ssStatus.dwCurrentState << std::endl;
        std::cout << "  Check Point: " << ssStatus.dwCheckPoint << std::endl;
        std::cout << "  Wait Hint: " << ssStatus.dwWaitHint << std::endl;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int SvcStop() {
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return 1;
    }

    schService = OpenService(
            schSCManager,         // SCM database
            SVCNAME,              // name of service
            SERVICE_STOP |
            SERVICE_QUERY_STATUS |
            SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED) {
        printf("Service is already stopped.\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
        printf("Service stop pending...\n");

        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&ssp,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED) {
            printf("Service stopped successfully.\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }

        if (GetTickCount() - dwStartTime > dwTimeout) {
            printf("Service stop timed out.\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }
    }

    if (!ControlService(
            schService,
            SERVICE_CONTROL_STOP,
            (LPSERVICE_STATUS) &ssp))
    {
        printf( "ControlService failed (%d)\n", GetLastError() );
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    while (ssp.dwCurrentState != SERVICE_STOPPED) {
        Sleep(ssp.dwWaitHint);
        if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&ssp,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
        {
            printf( "QueryServiceStatusEx failed (%d)\n", GetLastError() );
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout) {
            printf( "Wait timed out\n" );
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return 1;
        }
    }
    printf("Service stopped successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int SvcEnable() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return 1;
    }

    schService = OpenService(
            schSCManager,            // SCM database
            SVCNAME,                 // name of service
            SERVICE_CHANGE_CONFIG);  // need change config access

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (!ChangeServiceConfig(
            schService,            // handle of service
            SERVICE_NO_CHANGE,     // service type: no change
            SERVICE_DEMAND_START,  // service start type
            SERVICE_NO_CHANGE,     // error control: no change
            NULL,                  // binary path: no change
            NULL,                  // load order group: no change
            NULL,                  // tag ID: no change
            NULL,                  // dependencies: no change
            NULL,                  // account name: no change
            NULL,                  // password: no change
            NULL))                // display name: no change
    {
        printf("ChangeServiceConfig failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    printf("Service enabled successfully.\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int SvcDisable() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return 1;
    }

    schService = OpenService(
            schSCManager,            // SCM database
            SVCNAME,                 // name of service
            SERVICE_CHANGE_CONFIG);  // need change config access

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return 1;
    }

    if (!ChangeServiceConfig(
            schService,        // handle of service
            SERVICE_NO_CHANGE, // service type: no change
            SERVICE_DISABLED,  // service start type
            SERVICE_NO_CHANGE, // error control: no change
            NULL,              // binary path: no change
            NULL,              // load order group: no change
            NULL,              // tag ID: no change
            NULL,              // dependencies: no change
            NULL,              // account name: no change
            NULL,              // password: no change
            NULL) )            // display name: no change
    {
        printf("ChangeServiceConfig failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 1;
    }

    printf("Service disabled successfully.\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}
