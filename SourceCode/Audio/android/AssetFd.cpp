
#define LOG_TAG "AssetFd"

#include "audio/android/cutils/log.h"
#include "audio/android/AssetFd.h"

AssetFd::AssetFd(int assetFd)
        : _assetFd(assetFd)
{
}

AssetFd::~AssetFd()
{
    ALOGV("~AssetFd: %d", _assetFd);
    if (_assetFd > 0)
    {
        ::close(_assetFd);
        _assetFd = 0;
    }
};
