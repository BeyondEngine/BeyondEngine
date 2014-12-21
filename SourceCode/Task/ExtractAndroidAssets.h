#ifndef BEYOND_ENGINE_TASK_EXTRACTANDROIDASSETS_H__INCLUDE
#define BEYOND_ENGINE_TASK_EXTRACTANDROIDASSETS_H__INCLUDE
    #if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
        #include "TaskBase.h"

        struct SDirectory;
        class CExtractAndroidAssets : public CTaskBase
        {
        public:
            CExtractAndroidAssets();
            virtual ~CExtractAndroidAssets();

            virtual uint32_t GetProgress() override;
            virtual void Execute(float ddt) override;

            static void ExtractAssetsFromDirectory(SDirectory* pDirectory, uint32_t& extractSize);

        private:
            uint32_t m_uTotalSize;
            uint32_t m_uExtractedSize;
            SDirectory* m_pAssetsDirectory;
        };
    #endif
#endif