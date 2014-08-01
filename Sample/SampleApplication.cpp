#include "stdafx.h"
#include "SampleApplication.h"
#include "Task\TaskManager.h"
#include "Resource\ResourcePathManager.h"

CSampleApplication::CSampleApplication()
{
}

CSampleApplication::~CSampleApplication()
{
}

void CSampleApplication::Initialize()
{
    CApplication::Initialize();
    TString strResourcePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Resource);
    strResourcePath.append(_T("/AIData.bin"));
    CComponentInstanceManager* pComponentInstanceMgr = CComponentInstanceManager::GetInstance();
    CSerializer* pSerializer = pComponentInstanceMgr->Import(strResourcePath.c_str());
    BEATS_ASSERT(pSerializer->GetWritePos() > 0);
}