#include "stdafx.h"
#include "DeviceInfoGenerator.h"
#include "external/Configuration.h"
#include <WinInet.h>

#pragma comment(lib, "Wininet.lib")

void CConfiguration::GatherDeviceInfo()
{
    CDeviceInfoGenerator generator;
    m_osInfo = generator.GenerateOsVersion();
    m_modelInfo = generator.GenerateModelInfo();
    m_eDeviceType = eDT_Pad;
}

ENetState CConfiguration::GetNetworkState() const
{
    DWORD dwFlags = 0;
    if (InternetGetConnectedState(&dwFlags, 0))
    {
        if ((dwFlags & INTERNET_CONNECTION_LAN)
            || (dwFlags & INTERNET_CONNECTION_MODEM))
        {
            return ENetState::eNS_ETH;
        }
    }
    return ENetState::eNS_UNAVAILABLE;
}
