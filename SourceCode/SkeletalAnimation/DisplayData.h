#ifndef BEYOND_ENGINE_SKELETALANIMATION_DISPLAYDATA_H_INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_DISPLAYDATA_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

namespace _2DSkeletalAnimation
{
    enum EDisplayType
    {
        eST_CS_DISPLAY_SPRITE,
        eST_CS_DISPLAY_ARMATURE,
        eST_CS_DISPLAY_PARTICLE,
        eST_CS_DISPLAY_MAX,
    };

    class CDisplayData : public CComponentInstance
    {
        DECLARE_REFLECT_GUID(CDisplayData, 0x2F862C9C, CComponentInstance)
    public:
        CDisplayData();
        virtual ~CDisplayData();

        virtual void ReflectData(CSerializer& serializer) override;
        virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
        bool CheckChange();
        void SetChange(bool bChange);

        EDisplayType GetType() const;
        void SetType(EDisplayType type);

        const std::string& GetName() const;
        void  SetName(const std::string& name);

        float GetPositionX() const;
        void SetPositionX(float x);
        float GetPositionY() const;
        void SetPositionY(float y);

        float GetSkewX() const;
        void SetSkewX(float skewX);
        float GetSkewY() const;
        void SetSkewY(float skewY);

        float GetScaleX() const;
        void SetScaleX(float scaleX);
        float GetScaleY() const;
        void SetScaleY(float scaleY);
    private:
        EDisplayType m_eType;
        std::string m_strName;
        float m_fX;
        float m_fY;
        float m_fSkewX;
        float m_fSkewY;
        float m_fScaleX;
        float m_fScaleY;
        TString m_tstrName;
        bool m_bChanged;
    };
}

#endif//!SKELETALANIMATION_CDISPLAYDATA_H_INCLUDE
