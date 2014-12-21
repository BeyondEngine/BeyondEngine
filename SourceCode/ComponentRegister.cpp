#include "stdafx.h"
#include "Component/ComponentLauncher.h"
#include "Component/Component/ComponentInstanceManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Resource/ResourceManager.h"
#include "Resource/Resource.h"
#include "Render/Model.h"
#include "Render/RenderState.h"
#include "Render/Material.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/TextureAtlas.h"
#include "Render/StaticMesh.h"
#include "Render/AnimatableSprite.h"
#include "Render/AtlasSprite.h"
#include "Render/CircleProgressSprite.h"
//#include "TestComponent.h"
//scence
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Node2D.h"
#include "Scene/Node3D.h"

#include "Action/LaunchTask.h"
#include "Action/SetNodeVisible.h"
#include "Action/SetNodeActive.h"

// Node Animation
#include "NodeAnimation/NodeAnimationData.h"
#include "NodeAnimation/NodeAnimationElement.h"

// AI
#include "Task/TaskBase.h"
#include "GUI/Font/Font.h"
#include "Render/2DBackGround.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/BasicModule.h"
#include "ParticleSystem/ShapeModule.h"
#include "ParticleSystem/EmissionModule.h"
#include "ParticleSystem/ColorModule.h"
#include "ParticleSystem/ForceModule.h"
#include "ParticleSystem/RenderModule.h"
#include "ParticleSystem/RotationModule.h"
#include "ParticleSystem/SizeModule.h"
#include "ParticleSystem/TextureSheetAnimationModule.h"
#include "ParticleSystem/VelocityModule.h"
#include "ParticleSystem/VelocityLimitModule.h"

START_REGISTER_COMPONENT(EngineLaunch)

REGISTER_ABSTRACT_COMPONENT(CResource)
REGISTER_COMPONENT(CMaterial, _T("材质"), _T("Resource"))
REGISTER_COMPONENT(CFont, _T("字体"), _T("Resource"))
REGISTER_COMPONENT(CShader, _T("着色器"), _T("Resource"))
REGISTER_COMPONENT(CTexture, _T("纹理"), _T("Resource"))
REGISTER_COMPONENT(CTextureAtlas, _T("纹理集"), _T("Resource"))
REGISTER_COMPONENT(CStaticMesh, _T("静态模型"), _T("Resource"))
REGISTER_COMPONENT(CSprite, _T("精灵"), _T("Resource"))
REGISTER_COMPONENT(CAnimatableSprite, _T("单帧动画精灵"), _T("Resource"))
REGISTER_COMPONENT(CAtlasSprite, _T("独立帧动画精灵"), _T("Resource"))
REGISTER_COMPONENT(CCircleProgressSprite, _T("冷却条精灵"), _T("Resource"))
REGISTER_COMPONENT(CModel, _T("骨骼动画模型"), _T("Resource"))

REGISTER_COMPONENT(CScene, _T("场景"), _T("场景组件"))

REGISTER_COMPONENT(CNode2D, _T("2D节点"), _T("场景组件"))
REGISTER_COMPONENT(CNode3D, _T("3D节点"), _T("场景组件"))
REGISTER_COMPONENT(CNode, _T("节点"), _T("场景组件"))

//REGISTER_COMPONENT(CTestComponent, _T("TestComponent"),_T("TestComponent"))
//REGISTER_ABSTRACT_COMPONENT(CTestAbstractClass)

REGISTER_ABSTRACT_COMPONENT(CActionBase)
REGISTER_COMPONENT(CLaunchTask, _T("发起任务"), _T("行为"))

REGISTER_COMPONENT(CSetNodeVisible, _T("设置节点可见"), _T("行为"))
REGISTER_COMPONENT(CSetNodeActive, _T("设置节点激活"), _T("行为"))

REGISTER_ABSTRACT_COMPONENT(CTaskBase)

REGISTER_COMPONENT(CNodeAnimationData, _T("节点动画数据"), _T("节点动画"))
REGISTER_COMPONENT(CNodeAnimationElement, _T("属性动画"), _T(""))
REGISTER_COMPONENT(C2DBackGround, _T("2D背景"), _T("Render"))

// Particle System
REGISTER_COMPONENT(CParticleEmitter, _T("粒子发射器"), _T("Particle"))
REGISTER_COMPONENT(CBasicModule, _T("基本模块"), _T(""))
REGISTER_COMPONENT(CShapeModule, _T("发射形状模块"), _T(""))
REGISTER_COMPONENT(CEmissionModule, _T("喷射模块"), _T(""))
REGISTER_COMPONENT(CColorModule, _T("颜色模块"), _T(""))
REGISTER_COMPONENT(CForceModule, _T("外力模块"), _T(""))
REGISTER_COMPONENT(CRenderModule, _T("渲染模块"), _T(""))
REGISTER_COMPONENT(CRotationModule, _T("旋转模块"), _T(""))
REGISTER_COMPONENT(CSizeModule, _T("尺寸模块"), _T(""))
REGISTER_COMPONENT(CTextureSheetAnimationModule, _T("纹理动画模块"), _T(""))
REGISTER_COMPONENT(CVelocityModule, _T("速度模块"), _T(""))
REGISTER_COMPONENT(CVelocityLimitModule, _T("速度限制模块"), _T(""))

END_REGISTER_COMPONENT
