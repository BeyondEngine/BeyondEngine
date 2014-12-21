#ifndef BEYOND_ENGINE_TASK_EXTRACTIOSASSETS_H__INCLUDE
#define BEYOND_ENGINE_TASK_EXTRACTIOSASSETS_H__INCLUDE
    #if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
        #include "Task/TaskBase.h"

        struct SDirectory;
        class CExtractIosAssets : public CTaskBase
        {
        public:
            CExtractIosAssets();
            virtual ~CExtractIosAssets();

            virtual uint32_t GetProgress() override;
            virtual void Execute(float ddt) override;

            bool CopyOneFile(const std::string& strSrcPath, const std::string& strDesPath);
            void CopyFiles(const std::string& strSrcPath, const std::string& strDesPath);

            uint32_t fileSizeAtPath(TString filePath);
            uint32_t folderSizeAtPath(TString folderPath);
        private:
            uint32_t m_uTotalSize;
            uint32_t m_uExtractedSize;
            SDirectory* m_pAssetsDirectory;
        };
    #endif
#endif
