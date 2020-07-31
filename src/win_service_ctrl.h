/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#ifndef JWINSERVICE_WIN_SERVICE_CTRL_H
#define JWINSERVICE_WIN_SERVICE_CTRL_H

int SvcInstall();
int SvcUnistall();
int SvcStart();
int SvcStop();
int SvcEnable();
int SvcDisable();

#endif //JWINSERVICE_WIN_SERVICE_CTRL_H
