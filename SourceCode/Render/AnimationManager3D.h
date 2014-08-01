#ifndef BEYOND_ENGINE_ANIMATIONMANAGER_3D_H__INCLUDE
#define BEYOND_ENGINE_ANIMATIONMANAGER_3D_H__INCLUDE

class CBaseAnimationController;
class CAnimationController;
class CSpriteAnimationController;

class CAnimationManager3D
{
    BEATS_DECLARE_SINGLETON(CAnimationManager3D);
public:
    CAnimationController *CreateSkelAnimationController();
    void DeleteController(CBaseAnimationController *controller);

    void Update(float deltaTime);

private:
    std::list<CBaseAnimationController *> m_controllers;
};

#endif