/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#include <windows.h>
#include <iostream>
#include <io.h>
#include <tchar.h>

#include "conf_parser.h"

int readServiceName(LPTSTR serviceName, size_t maxServiceNameSize) {
    if (_access(CONF_FILE, 0) != 0) {
        std::cout << "Cannot find configuration file " CONF_FILE << std::endl;
        return 1;
    }

    LPCTSTR path = _T(".\\" CONF_FILE);
    int size = GetPrivateProfileString(_T(CONF_INI_SECTION_SERVICE), _T(CONF_INI_VALUE_SERVICE_NAME), _T(""), serviceName, maxServiceNameSize, path);
    if (size == 0) {
        std::cout << "Cannot read " CONF_INI_VALUE_SERVICE_NAME " from configuration file" << std::endl;
        return 1;
    }

    return 0;
}
