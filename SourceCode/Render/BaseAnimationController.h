#ifndef BEYOND_ENGINE_RENDER_BASEANIMATIONCONTROLLER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_BASEANIMATIONCONTROLLER_H__INCLUDE

class CBaseAnimationController
{
public:
    virtual ~CBaseAnimationController(){}
    virtual void Update(float deltaTime) = 0;
};

#endif