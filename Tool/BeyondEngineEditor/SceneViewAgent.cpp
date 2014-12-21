#include "stdafx.h"
#include "SceneViewAgent.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorGLWindow.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "Render/Viewport.h"
#include "Vec3fPropertyDescription.h"
#include "WxGLRenderWindow.h"
#include "Render/Sprite.h"
#include "Render/Camera.h"
#include "Scene/SceneManager.h"

CSceneViewAgent* CSceneViewAgent::m_pInstance = nullptr;
CSceneViewAgent::CSceneViewAgent()
{

}

CSceneViewAgent::~CSceneViewAgent()
{

}

void CSceneViewAgent::InView()
{
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pResourcePanel).Show();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();
    m_pMainFrame->m_Manager.Update();
}