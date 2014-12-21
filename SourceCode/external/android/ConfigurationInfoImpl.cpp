#include "stdafx.h"
#include "external/Configuration.h"
#include "Framework/android/AndroidHandler.h"

void CConfiguration::GatherDeviceInfo()
{
    CAndroidHandler* pAndroidHandler = CAndroidHandler::GetInstance();
    m_osInfo = pAndroidHandler->GetOsVersion();
    m_modelInfo = pAndroidHandler->GetModelInfo();
    m_eDeviceType = pAndroidHandler->GetDeviceType() == 1 ? EDeviceType::eDT_Pad : EDeviceType::eDT_Phone;
}

ENetState CConfiguration::GetNetworkState() const
{
    ENetState ret = ENetState::eNS_UNAVAILABLE;
    CAndroidHandler* pAndroidHandler = CAndroidHandler::GetInstance();
    int nState = pAndroidHandler->GetNetState();
    switch (nState)
    {
    case -1:
        ret = ENetState::eNS_UNAVAILABLE;
        break;
    case 1:
        ret = ENetState::eNS_WIFI;
        break;
    case 2:
        ret = ENetState::eNS_MOBILE;
        break;
    default:
        break;
    }
    return ret;
}

