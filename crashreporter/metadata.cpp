/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "metadata.h"

bool Metadata::valid()
{
    return !(uploadUrl.empty()
             || productName.empty()
             || productVersion.empty()
             || clientGuid.empty()
             || dumpPath.empty());
}
