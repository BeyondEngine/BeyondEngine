#ifndef BEYOND_ENGINE_ACTION_PLAYMUSIC_H__INCLUDE
#define BEYOND_ENGINE_ACTION_PLAYMUSIC_H__INCLUDE

#include "ActionBase.h"

class CPlayMusic : public CActionBase
{
    DECLARE_REFLECT_GUID( CPlayMusic, 0x17D011B3, CActionBase )
public:
    CPlayMusic();
    virtual ~CPlayMusic();

    size_t GetFadeInTime() const;
    void SetFadeInTime(size_t uTimeMS);
    float GetVolume() const;
    void SetVolume(float fVolume);

    virtual void Initialize() override;
    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    bool m_bLoop;
    bool m_bRewind;
    float m_fVolume;
    size_t m_uFadeInTimeMS;
    TString m_strFileName;
};

#endif