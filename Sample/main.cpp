#include "PlatformConfig.h"
#include "BeatsTchar.h"
#if(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    #include <android_native_app_glue.h>
    #include <android/log.h>
#endif
#include "StarRaidersApplication.h"
#include "EnginePublic/PublicDef.h"

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    int APIENTRY _tWinMain(HINSTANCE /*hInstance*/,
                           HINSTANCE /*hPrevInstance*/,
                           LPTSTR    /*lpCmdLine*/,
                           int       /*nCmdShow*/)

#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    int main(int argc, char * argv[])
#elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    void android_main( android_app* state )
#elif (BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
    int main(int argc, char* argv[])
#else
    #error ("unsupport platform!")
#endif
{
    CStarRaidersApplication app;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    return app.Run(0, nullptr, _T("width:960, height:640"));
#elif (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    //Don't remove this function, it ensure that glue won't be optimized!
    app_dummy();
    app.Run(0, nullptr, state);
#else
    return app.Run(argc, argv, nullptr);
#endif
}
