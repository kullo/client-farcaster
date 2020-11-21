/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
#include "metadata.h"

bool Metadata::valid()
{
    return !(uploadUrl.empty()
             || productName.empty()
             || productVersion.empty()
             || clientGuid.empty()
             || dumpPath.empty());
}
