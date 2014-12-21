#ifndef CDEVICEINFOGENERATOR_H_INCLUDE
#define CDEVICEINFOGENERATOR_H_INCLUDE

class CDeviceInfoGenerator
{
public:
    TString GenerateOsVersion();
    TString GenerateModelInfo();
protected:
    BOOL isWinVersionLessThan(DWORD dwMajorVersion, DWORD dwMinorVersion);
    BOOL isWinVersionGreaterThan(DWORD dwMajorVersion, DWORD dwMinorVersion);
    BOOL isWinVersionEqualTo(DWORD dwMajorVersion, DWORD dwMinorVersion);
};
#endif