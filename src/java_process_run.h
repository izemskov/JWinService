/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#ifndef JWINSERVICE_JAVA_PROCESS_RUN_H
#define JWINSERVICE_JAVA_PROCESS_RUN_H

extern STARTUPINFO          si;
extern PROCESS_INFORMATION  pi;

int runJavaProcess();

#endif //JWINSERVICE_JAVA_PROCESS_RUN_H
