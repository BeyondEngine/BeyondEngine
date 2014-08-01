#include "stdafx.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentLauncher.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstanceManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Resource/ResourcePathManager.h"
#include "Resource/Resource.h"
#include "Render/Model.h"
#include "Render/Skeleton.h"
#include "Render/RenderState.h"
#include "Render/Material.h"
#include "Render/Animation3D.h"
#include "Render/Shader.h"
#include "Render/Skin.h"
#include "Render/Texture.h"
#include "Render/TextureAtlas.h"
#include "Render/StaticMesh.h"
#include "Render/AnimatableSprite.h"
#include "Render/FrameSprite.h"

#include "GUI/Window/Control.h"
#include "GUI/Window/TextureControl.h"
#include "GUI/Window/Window.h"
#include "GUI/Window/Button.h"
#include "GUI/Window/ImageView.h"
#include "GUI/Window/ProgressBar.h"
#include "GUI/Window/Label.h"
#include "GUI/Window/TopWindow.h"
#include "GUI/Window/TabWindow.h"
#include "GUI/Window/ScrollItem.h"
#include "GUI/Window/ScrollView.h"
#include "GUI/Window/TipWindow.h"
#include "GUI/Window/TextBox.h"
#include "GUI/Font/Font.h"

#include "ParticlesSystem/ParticleSystem.h"
#include "ParticlesSystem/Emitter.h"
#include "ParticlesSystem/ColorParticleAnimation.h"
#include "ParticlesSystem/RotateParticleAnimation.h"
#include "ParticlesSystem/ScaleParticleAnimation.h"

#include "SkeletalAnimation/AnimationModel.h"
#include "SkeletalAnimation/AnimationData.h"
#include "SkeletalAnimation/SkeletonAnimationController.h"
#include "SkeletalAnimation/ArmatureData.h"
#include "SkeletalAnimation/DisplayData.h"

#include "TestComponent.h"
//scence
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Node2D.h"
#include "Scene/Node3D.h"

#include "Action/ConditionActionBase.h"
#include "Action/SwitchScene.h"
#include "Action/LaunchTask.h"
#include "Action/PlayMusic.h"
#include "Action/PlayEffect.h"
#include "Action/StopMusic.h"
#include "Action/StopEffect.h"
#include "Action/ChangeAIState.h"
#include "Task/ChangeCameraFov.h"

// Node Animation
#include "NodeAnimation/NodeAnimationData.h"
#include "NodeAnimation/NodeAnimationElement.h"

// AI
#include "AI/AIScheme.h"
#include "AI/AIState.h"

START_REGISTER_COMPONENT(EngineLaunch)
REGISTER_ABSTRACT_COMPONENT(CResource)
REGISTER_COMPONENT(CSkeleton, _T("骨骼"), _T("Resource"))
REGISTER_COMPONENT(CMaterial, _T("材质"), _T("Resource"))
REGISTER_COMPONENT(CAnimation3D, _T("动画"), _T("Resource"))
REGISTER_COMPONENT(CFont, _T("字体"), _T("Resource"))
REGISTER_COMPONENT(CShader, _T("着色器"), _T("Resource"))
REGISTER_COMPONENT(CSkin, _T("蒙皮"), _T("Resource"))
REGISTER_COMPONENT(CTexture, _T("纹理"), _T("Resource"))
REGISTER_COMPONENT(CTextureAtlas, _T("纹理集"), _T("Resource"))
REGISTER_COMPONENT( CStaticMesh, _T("静态模型"), _T("Resource"))
REGISTER_COMPONENT( CSprite, _T("精灵"), _T("Resource"))
REGISTER_COMPONENT( CAnimatableSprite, _T("单帧动画精灵"), _T("Resource"))
REGISTER_COMPONENT( CFrameSprite, _T("多帧动画精灵"), _T("Resource"))
REGISTER_COMPONENT( CModel, _T("骨骼动画模型"), _T("Resource"))

//REGISTER_COMPONENT(_2DSkeletalAnimation::CAnimationModel,_T("动画模型"),_T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CAnimationData, _T("动画"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CSkeletonAnimationController, _T("动画控制器"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CArmatureData, _T("骨架"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CDisplayData, _T("蒙皮数据"), _T("2DSkeletalAnimation"));

REGISTER_COMPONENT(CControl, _T("Control"), _T("GUI\\Control"))
REGISTER_COMPONENT(CTextureControl, _T("TextureControl"), _T("GUI\\Control"))
REGISTER_COMPONENT(CWindow, _T("Window"), _T("GUI\\Control"))
REGISTER_COMPONENT(CButton, _T("Button"), _T("GUI\\Control"))
REGISTER_COMPONENT(CImageView, _T("ImageView"), _T("GUI\\Control"))
REGISTER_COMPONENT(CProgressBar, _T("ProgressBar"), _T("GUI\\Control"))
REGISTER_COMPONENT(CLabel, _T("Label"), _T("GUI\\Control"))
REGISTER_COMPONENT(CTopWindow, _T("TopWindow"), _T("GUI\\Control"))
REGISTER_COMPONENT(CTabWindow, _T("TabWindow"), _T("GUI\\Control"))
REGISTER_ABSTRACT_COMPONENT(CScrollItem)
REGISTER_COMPONENT(CScrollView, _T("ScrollView"), _T("GUI\\Control"))
REGISTER_COMPONENT(CTipWindow, _T("TipWindow"), _T("GUI\\Control"))
REGISTER_COMPONENT(CTextBox, _T("TextBox"), _T("GUI\\Control"))

//REGISTER_COMPONENT(NBeyondEngine::CParticleSystem, _T("粒子系统"), _T("粒子组件"))
//REGISTER_COMPONENT(NBeyondEngine::CParticleEmitter, _T("发射器"), _T("粒子组件") )
//REGISTER_COMPONENT(NBeyondEngine::CColorParticleAnimation, _T("颜色动画"), _T("粒子组件\\粒子动画类型") )
//REGISTER_COMPONENT(NBeyondEngine::CScaleParticleAnimation, _T("缩放动画"), _T("粒子组件\\粒子动画类型") )
//REGISTER_COMPONENT(NBeyondEngine::CRotateParticleAnimation, _T("旋转动画"), _T("粒子组件\\粒子动画类型") )
//REGISTER_ABSTRACT_COMPONENT( NBeyondEngine::CParticleAnimationBase )

REGISTER_COMPONENT(CShaderUniform, _T("ShaderUniform"), _T("渲染"))

REGISTER_COMPONENT( CScene, _T("场景"), _T("场景组件"))

REGISTER_COMPONENT( CNode2D, _T("2D节点"), _T("场景组件"))
REGISTER_COMPONENT( CNode3D, _T("3D节点"), _T("场景组件"))
REGISTER_COMPONENT( CNode, _T("节点"), _T("场景组件"))

REGISTER_COMPONENT(CTestComponent, _T("TestComponent"),_T("TestComponent"))
REGISTER_ABSTRACT_COMPONENT(CTestAbstractClass)

REGISTER_ABSTRACT_COMPONENT(CActionBase)
REGISTER_ABSTRACT_COMPONENT(CConditionActionBase)
REGISTER_COMPONENT(CSwitchScene, _T("切换场景"),_T("行为"))
REGISTER_COMPONENT(CLaunchTask, _T("发起任务"),_T("行为"))
REGISTER_COMPONENT(CPlayMusic, _T("播放音乐"),_T("行为"))
REGISTER_COMPONENT(CPlayEffect, _T("播放音效"),_T("行为"))
REGISTER_COMPONENT(CStopMusic, _T("停止音乐"),_T("行为"))
REGISTER_COMPONENT(CStopEffect, _T("停止音效"),_T("行为"))
REGISTER_COMPONENT(CChangeAIState, _T("跳转AI状态"), _T("行为"))

REGISTER_ABSTRACT_COMPONENT(CTaskBase)
REGISTER_COMPONENT(CChangeCameraFov, _T("改变FOV"),_T("任务"))

REGISTER_COMPONENT(CNodeAnimationData, _T("节点动画"), _T("节点动画"))
REGISTER_COMPONENT(CNodeAnimationElement, _T("属性动画"), _T("节点动画"))

REGISTER_COMPONENT(CAIScheme, _T("AI策略"), _T("AI"))
REGISTER_COMPONENT(CAIState, _T("AI状态"), _T("AI"))

END_REGISTER_COMPONENT
