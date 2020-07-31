/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#include <windows.h>
#include <tchar.h>
#include <iostream>

#include "conf_parser.h"
#include "win_service.h"
#include "win_service_ctrl.h"

void printUsages() {
    std::cout << "Usage: " << std::endl;
    std::cout << "  JWinService.exe [-command]" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  -install   - install the service to Service Control Manager" << std::endl;
    std::cout << "  -uninstall - uninstall the service" << std::endl;
    std::cout << "  -start     - start the service" << std::endl;
    std::cout << "  -stop      - stop the service" << std::endl;
    std::cout << "  -enable    - enable service" << std::endl;
    std::cout << "  -disable   - disable service" << std::endl;
}

int __cdecl _tmain(int argc, TCHAR *argv[]) {
    int status = readServiceName(SVCNAME, 255) != 0;
    if (status != 0)
        return status;

    if (argc > 1) {
        if (lstrcmpi(argv[1], TEXT("-install")) == 0) {
            return SvcInstall();
        }
        else if (lstrcmpi(argv[1], TEXT("-uninstall")) == 0) {
            return SvcUnistall();
        }
        else if (lstrcmpi(argv[1], TEXT("-start")) == 0) {
            return SvcStart();
        }
        else if (lstrcmpi(argv[1], TEXT("-stop")) == 0) {
            return SvcStop();
        }
        else if (lstrcmpi(argv[1], TEXT("-enable")) == 0) {
            return SvcEnable();
        }
        else if (lstrcmpi(argv[1], TEXT("-disable")) == 0) {
            return SvcDisable();
        }
        else {
            std::cout << "Unknown command" << std::endl << std::endl;
            printUsages();
            return 1;
        }
    }

    SERVICE_TABLE_ENTRY DispatchTable[] =
            {
                    { SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
                    { NULL, NULL }
            };

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
        return -1;
    }

    return 0;
}
