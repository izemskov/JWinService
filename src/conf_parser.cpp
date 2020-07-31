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

TCHAR CONF_FILE[] = _T("jwinservice.ini");

bool findJavaInRegistry = false;
TCHAR javaArguments[255];

static int getConfigFullPath(TCHAR * configFullPath) {
    TCHAR szPath[MAX_PATH];
    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        std::cout << "Cannot find executable path " << GetLastError() << std::endl;
        return 1;
    }

    TCHAR * lastSlash = _tcsrchr(szPath, '\\');
    if (lastSlash == NULL) {
        std::cout << "Cannot get current directory path" << std::endl;
        return 1;
    }

    _tcsncpy(configFullPath, szPath, lastSlash - szPath + 1);
    configFullPath[lastSlash - szPath + 1] = _T('\0');

    _tcscat_s(configFullPath, MAX_PATH, CONF_FILE);

    return 0;
}

int readServiceName(LPTSTR serviceName, size_t maxServiceNameSize) {
    TCHAR configFullPath[MAX_PATH];

    int status = getConfigFullPath(configFullPath);
    if (status != 0)
        return status;

    if (_access(configFullPath, 0) != 0) {
        std::cout << "Cannot find configuration file " << CONF_FILE << std::endl;
        return 1;
    }

    int size = GetPrivateProfileString(_T(CONF_INI_SECTION_SERVICE), _T(CONF_INI_VALUE_SERVICE_NAME), _T(""), serviceName, maxServiceNameSize, configFullPath);
    if (size == 0) {
        std::cout << "Cannot read " CONF_INI_VALUE_SERVICE_NAME " from configuration file" << std::endl;
        return 1;
    }

    TCHAR findJavaInRegistryStr[MAX_PATH];
    size = GetPrivateProfileString(_T(CONF_INI_SECTION_SERVICE), _T(CONF_INI_VALUE_FIND_JAVA_IN_REGISTRY), _T(""), findJavaInRegistryStr, MAX_PATH, configFullPath);
    if (size == 0) {
        std::cout << "Cannot read " CONF_INI_VALUE_FIND_JAVA_IN_REGISTRY " from configuration file" << std::endl;
        return 1;
    }
    if (lstrcmpi(findJavaInRegistryStr, TEXT("true"))== 0) {
        findJavaInRegistry = true;
    }

    size = GetPrivateProfileString(_T(CONF_INI_SECTION_SERVICE), _T(CONF_INI_VALUE_JAVA_ARGUMENTS), _T(""), javaArguments, MAX_PATH, configFullPath);
    if (size == 0) {
        std::cout << "Cannot read " CONF_INI_VALUE_JAVA_ARGUMENTS " from configuration file" << std::endl;
        return 1;
    }

    return 0;
}
