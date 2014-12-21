
#pragma once

#include <unistd.h>

class AssetFd
{
public:
    AssetFd(int assetFd);
    ~AssetFd();

    inline int getFd() const { return _assetFd; };
private:
    int _assetFd;
};

