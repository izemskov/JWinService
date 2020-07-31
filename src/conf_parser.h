/* This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2020 Ilya Zemskov */

#ifndef JWINSERVICE_CONF_PARSER_H
#define JWINSERVICE_CONF_PARSER_H

#define CONF_INI_SECTION_SERVICE    "Service"
#define CONF_INI_VALUE_SERVICE_NAME "ServiceName"

int readServiceName(LPTSTR serviceName, size_t maxServiceNameSize);

#endif //JWINSERVICE_CONF_PARSER_H
