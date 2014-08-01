#ifndef BEYOND_ENGINE_SKELETALANIMATION_BONEDATA_H__INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_BONEDATA_H__INCLUDE

namespace _2DSkeletalAnimation
{
    class CDisplayData;
    class  CBoneData
    {
    public:
        CBoneData();
        ~CBoneData();

        const std::string& GetName() const;
        void  SetName(const std::string& name);
        const std::string& GetParentName() const;
        void  SetParentName(const std::string& name);

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

       int GetZOrder() const;
       void SetZOrder(int zOrder);

       const std::vector<CDisplayData>& GetDisplayDataList() const;
       void AddDisplayData(const CDisplayData& data);
       CDisplayData ReplaceDisplayData(const CDisplayData& displayData, size_t index = 0);
    private:
        std::string m_strName;
        std::string m_parentName;
        float m_fX;
        float m_fY;
        int m_iZOrder;
        float m_fSkewX;
        float m_fSkewY;
        float m_fScaleX;
        float m_fScaleY;
        std::vector<CDisplayData> m_displayDataList;
    };

}
#endif//SKELETALANIMATION_CBONEDATA_H__INCLUDE
