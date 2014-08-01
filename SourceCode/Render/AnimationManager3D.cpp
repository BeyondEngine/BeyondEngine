#include "stdafx.h"
#include "AnimationManager3D.h"
#include "AnimationController.h"

CAnimationManager3D *CAnimationManager3D::m_pInstance = nullptr;

CAnimationManager3D::CAnimationManager3D()
{

}

CAnimationManager3D::~CAnimationManager3D()
{
    for(auto controller : m_controllers)
    {
        BEATS_SAFE_DELETE(controller);
    }
}

CAnimationController * CAnimationManager3D::CreateSkelAnimationController()
{
    CAnimationController *controller = new CAnimationController();
    m_controllers.push_back(controller);
    return controller;
}

void CAnimationManager3D::Update( float deltaTime )
{
    for(auto controller : m_controllers)
    {
        controller->Update(deltaTime);
    }
}

void CAnimationManager3D::DeleteController( CBaseAnimationController *controller )
{
    auto itr = find(m_controllers.begin(), m_controllers.end(), controller);
    if(itr != m_controllers.end())
    {
        BEATS_SAFE_DELETE(controller);
        m_controllers.erase(itr);
    }
}
