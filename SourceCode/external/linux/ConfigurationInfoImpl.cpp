#include "stdafx.h"
#include "external/Configuration.h"

void CConfiguration::GatherDeviceInfo()
{
}

ENetState CConfiguration::GetNetworkState() const
{
    ENetState ret = ENetState::eNS_UNAVAILABLE;
    return ret;
}

