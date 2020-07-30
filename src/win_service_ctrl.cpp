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
        SERVICE_DEMAND_START,      // start type
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
}

int SvcUnistall() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus;

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
}
