#include "stdafx.h"
#include "Framework/Application.h"
#include <android_native_app_glue.h>
#include <NDKHelper.h>
#include "Utility/BeatsUtility/FilePathTool.h"
#include "AndroidHandler.h"

int CApplication::Run(void* pData)
{
    BEATS_ASSERT(pData != NULL, _T("android application can't be null!"));
    android_app* pApp = (android_app*)pData;
    CAndroidHandler* pAndroidHandler = CAndroidHandler::GetInstance();
    pAndroidHandler->Initialize(pApp);
    CFilePathTool::GetInstance()->SetAssetManager(pApp->activity->assetManager);
    pApp->userData = nullptr;
    pApp->onAppCmd = CAndroidHandler::HandleCmd;
    pApp->onInputEvent = CAndroidHandler::HandleInput;
    // loop waiting for stuff to do.
    while( 1 )
    {
        // Read all pending events.
        int id;
        int events;
        android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while( (id = ALooper_pollAll( pAndroidHandler->IsFocus() ? 0 : -1, NULL, &events, (void**) &source )) >= 0 )
        {
            // Process this event.
            if( source != NULL )
                source->process( pApp, source );

            pAndroidHandler->ProcessSensors( id );

            // Check if we are exiting.
            if( pApp->destroyRequested != 0 )
            {
                pAndroidHandler->GetGLContext()->Suspend();
                return 0;
            }
        }
        CEngineCenter* pEngineCenter = CEngineCenter::GetInstance();
        if(pAndroidHandler->IsFocus() && pEngineCenter->ShouldUpdateThisFrame())
        {
            CApplication * pApplication = CApplication::GetInstance();
            pApplication->Update();
            pApplication->Render();
            pAndroidHandler->GetGLContext()->Swap();
        }
    }
    return 0;
}