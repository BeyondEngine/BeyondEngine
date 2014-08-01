#include "stdafx.h"
#include "SwitchScene.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

CSwitchScene::CSwitchScene()
    : m_uFileId(0xFFFFFFFF)
{

}

CSwitchScene::~CSwitchScene()
{

}

bool CSwitchScene::ExecuteImp(SActionContext* /*pContext*/)
{
    CEngineCenter::GetInstance()->SwitchScene(m_uFileId);
    return false;
}

void CSwitchScene::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_uFileId, true, 0xFF0000FF, _T("Ä¿±ê³¡¾°ID"), NULL, NULL, NULL);
}

