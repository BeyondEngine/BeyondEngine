#include "PlatformConfig.h"
#include "BeatsPlatFormConfig.h"
#include "BeatsTchar.h"
#if(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    #include <android_native_app_glue.h>
    #include <android/log.h>
#endif
#include "SampleApplication.h"
#include "PublicDef.h"

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    int APIENTRY _tWinMain(HINSTANCE /*hInstance*/,
                           HINSTANCE /*hPrevInstance*/,
                           LPTSTR    /*lpCmdLine*/,
                           int       /*nCmdShow*/)

#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    int main(int argc, char * argv[])
#elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    void android_main( android_app* state )
#else
    #error ("unsupport platform!")
#endif
{
    CSampleApplication app;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    return app.Run(_T("width:960, height:640"));
#elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    //Don't remove this function, it ensure that glue won't be optimized!
    app_dummy();
    app.Run(state);
#else
    return app.Run();
#endif
}
