/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#include <windows.h>
#include <string>
#include <iostream>
#include <tchar.h>

#include "conf_parser.h"

#include "java_process_run.h"

STARTUPINFO          si;
PROCESS_INFORMATION  pi;

static bool isWow64() {
    BOOL bIsWow64 = FALSE;
    typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS fnIsWow64Process =
            (LPFN_ISWOW64PROCESS)GetProcAddress(
                    GetModuleHandle(TEXT("kernel32")),
                    "IsWow64Process"
            );

    if (NULL != fnIsWow64Process && !fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
        bIsWow64 = FALSE;
    }
    return bIsWow64;
}

static int getRegStrValue(const char * pKey, const char* pName, std::string& Value, bool useWOW64) {
    UINT  r;
    HKEY  reg_key;
    char  b[_MAX_PATH+1];
    DWORD data_type;
    DWORD bl     = sizeof(b);
    int status = 1;

    r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pKey, 0, KEY_READ | (useWOW64 ? KEY_WOW64_64KEY : KEY_WOW64_32KEY), &reg_key);
    if (r == ERROR_SUCCESS) {
        r = RegQueryValueEx(reg_key, pName, NULL, &data_type, (LPBYTE)b, &bl);
        if (r == ERROR_SUCCESS) {
            if (REG_SZ == data_type) status = 0;
            else                     status = 2;
        }
        else if (r = ERROR_FILE_NOT_FOUND) status = 2;

        RegCloseKey(reg_key);
    }
    else if (r = ERROR_FILE_NOT_FOUND) status = 2;

    if (0 == status) Value = b;
    return status;
}

static int getJavaPathFromRegistry(std::string & javaPath) {
#ifdef _WIN64
    bool isWin64 = true;
#else
    bool isWin64 = isWow64();
#endif
    bool isJavaX64 = isWin64;

    std::string str;
    std::string path = "SOFTWARE\\JavaSoft\\Java Runtime Environment";
    if (getRegStrValue(path.c_str(), "CurrentVersion", str, isWin64) != 0)
    {
        if (!isWin64) {
            std::cout << "Cannot find current version Java" << std::endl;
            return 1;
        }
        else if (getRegStrValue(path.c_str(), "CurrentVersion", str, FALSE) != 0) {
            std::cout << "Cannot find current version Java" << std::endl;
            return 1;
        }

        isJavaX64 = FALSE;
    }

    path += std::string("\\") + str;
    if (getRegStrValue(path.c_str(), "JavaHome", javaPath, isJavaX64) != 0) {
        std::cout << "Cannot find JavaHome folder" << std::endl;
        return 1;
    }

    return 0;
}

static int getCurrentDirectory(TCHAR * configFullPath) {
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

    _tcsncpy(configFullPath, szPath, lastSlash - szPath);
    configFullPath[lastSlash - szPath] = _T('\0');

    return 0;
}

int runJavaProcess() {
    std::string javaPath = "java";
    if (findJavaInRegistry) {
        if (getJavaPathFromRegistry(javaPath) != 0) {
            return 1;
        }
        javaPath += "\\bin\\java.exe";
    }

    std::string str = std::string("\"") + javaPath + "\" " + javaArguments;

    TCHAR currentDirectory[255];
    if (getCurrentDirectory(currentDirectory) != 0) {
        return 1;
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    GetStartupInfo(&si);

    if (!CreateProcess(
            NULL,                         // pointer to name of executable module
            (LPTSTR)str.c_str(),          // pointer to command line string
            NULL,                         // pointer to process security attributes
            NULL,                         // pointer to thread security attributes
            FALSE,                        // handle inheritance flag
            CREATE_NEW_CONSOLE,             // creation flags
            NULL,                         // pointer to new environment block
            currentDirectory,             // pointer to current directory name
            &si,                          // pointer to STARTUPINFO
            &pi                           // pointer to PROCESS_INFORMATION
    ))
    {
        std::cout << "Cannot correctly run: " << str << " Error code: " << GetLastError() << std::endl;
        return 1;
    }

    return 0;
}
