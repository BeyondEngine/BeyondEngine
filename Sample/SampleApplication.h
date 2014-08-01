#ifndef __BEYONDEngine__SampleApplication__
#define __BEYONDEngine__SampleApplication__

#include "Framework/Application.h"

class CSampleApplication : public CApplication
{
public:
    CSampleApplication();
    virtual ~CSampleApplication();
    virtual void Initialize() override;
};

#endif
