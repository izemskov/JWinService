/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <io.h>

#include "win_service.h"

int __cdecl _tmain(int argc, TCHAR *argv[])
{
    if (_access("jwinservice.ini", 0) != 0) {
        std::cout << "Cannot find configuration file jwinservice.ini";
        return -1;
    }

    /*// If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.

    if (lstrcmpi(argv[1], TEXT("install")) == 0)
    {
        SvcInstall();
        return 0;
    }*/

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
