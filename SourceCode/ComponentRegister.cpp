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
REGISTER_COMPONENT(CSkeleton, _T("����"), _T("Resource"))
REGISTER_COMPONENT(CMaterial, _T("����"), _T("Resource"))
REGISTER_COMPONENT(CAnimation3D, _T("����"), _T("Resource"))
REGISTER_COMPONENT(CFont, _T("����"), _T("Resource"))
REGISTER_COMPONENT(CShader, _T("��ɫ��"), _T("Resource"))
REGISTER_COMPONENT(CSkin, _T("��Ƥ"), _T("Resource"))
REGISTER_COMPONENT(CTexture, _T("����"), _T("Resource"))
REGISTER_COMPONENT(CTextureAtlas, _T("����"), _T("Resource"))
REGISTER_COMPONENT( CStaticMesh, _T("��̬ģ��"), _T("Resource"))
REGISTER_COMPONENT( CSprite, _T("����"), _T("Resource"))
REGISTER_COMPONENT( CAnimatableSprite, _T("��֡��������"), _T("Resource"))
REGISTER_COMPONENT( CFrameSprite, _T("��֡��������"), _T("Resource"))
REGISTER_COMPONENT( CModel, _T("��������ģ��"), _T("Resource"))

//REGISTER_COMPONENT(_2DSkeletalAnimation::CAnimationModel,_T("����ģ��"),_T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CAnimationData, _T("����"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CSkeletonAnimationController, _T("����������"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CArmatureData, _T("�Ǽ�"), _T("2DSkeletalAnimation"));
//REGISTER_COMPONENT(_2DSkeletalAnimation::CDisplayData, _T("��Ƥ����"), _T("2DSkeletalAnimation"));

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

//REGISTER_COMPONENT(NBeyondEngine::CParticleSystem, _T("����ϵͳ"), _T("�������"))
//REGISTER_COMPONENT(NBeyondEngine::CParticleEmitter, _T("������"), _T("�������") )
//REGISTER_COMPONENT(NBeyondEngine::CColorParticleAnimation, _T("��ɫ����"), _T("�������\\���Ӷ�������") )
//REGISTER_COMPONENT(NBeyondEngine::CScaleParticleAnimation, _T("���Ŷ���"), _T("�������\\���Ӷ�������") )
//REGISTER_COMPONENT(NBeyondEngine::CRotateParticleAnimation, _T("��ת����"), _T("�������\\���Ӷ�������") )
//REGISTER_ABSTRACT_COMPONENT( NBeyondEngine::CParticleAnimationBase )

REGISTER_COMPONENT(CShaderUniform, _T("ShaderUniform"), _T("��Ⱦ"))

REGISTER_COMPONENT( CScene, _T("����"), _T("�������"))

REGISTER_COMPONENT( CNode2D, _T("2D�ڵ�"), _T("�������"))
REGISTER_COMPONENT( CNode3D, _T("3D�ڵ�"), _T("�������"))
REGISTER_COMPONENT( CNode, _T("�ڵ�"), _T("�������"))

REGISTER_COMPONENT(CTestComponent, _T("TestComponent"),_T("TestComponent"))
REGISTER_ABSTRACT_COMPONENT(CTestAbstractClass)

REGISTER_ABSTRACT_COMPONENT(CActionBase)
REGISTER_ABSTRACT_COMPONENT(CConditionActionBase)
REGISTER_COMPONENT(CSwitchScene, _T("�л�����"),_T("��Ϊ"))
REGISTER_COMPONENT(CLaunchTask, _T("��������"),_T("��Ϊ"))
REGISTER_COMPONENT(CPlayMusic, _T("��������"),_T("��Ϊ"))
REGISTER_COMPONENT(CPlayEffect, _T("������Ч"),_T("��Ϊ"))
REGISTER_COMPONENT(CStopMusic, _T("ֹͣ����"),_T("��Ϊ"))
REGISTER_COMPONENT(CStopEffect, _T("ֹͣ��Ч"),_T("��Ϊ"))
REGISTER_COMPONENT(CChangeAIState, _T("��תAI״̬"), _T("��Ϊ"))

REGISTER_ABSTRACT_COMPONENT(CTaskBase)
REGISTER_COMPONENT(CChangeCameraFov, _T("�ı�FOV"),_T("����"))

REGISTER_COMPONENT(CNodeAnimationData, _T("�ڵ㶯��"), _T("�ڵ㶯��"))
REGISTER_COMPONENT(CNodeAnimationElement, _T("���Զ���"), _T("�ڵ㶯��"))

REGISTER_COMPONENT(CAIScheme, _T("AI����"), _T("AI"))
REGISTER_COMPONENT(CAIState, _T("AI״̬"), _T("AI"))

END_REGISTER_COMPONENT
