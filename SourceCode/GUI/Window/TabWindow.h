#ifndef BEYOND_ENGINE_GUI_TABWINDOW_H__INCLUDE
#define BEYOND_ENGINE_GUI_TABWINDOW_H__INCLUDE

#include "Control.h"
#include "Render/ReflectTextureInfo.h"

class CButton;
class CLabel;
class CBaseEvent;

enum ETabButtonSideType
{
    eTS_Top = 0,
    eTS_Bottom,
};

enum ETabButtonPositionType
{
    eTP_Left = 0,
    eTP_Center,
    eTP_Right,
};

class CTabWindow : public CControl
{
    DECLARE_REFLECT_GUID( CTabWindow, 0x1458a907, CControl )
public:
    CTabWindow();
    virtual ~CTabWindow();

    virtual void Initialize() override;

    virtual void Uninitialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual bool OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent) override;

    virtual void SetSize( const CVec2& size ) override;

    void OnTabButtonClicked( CBaseEvent* event );

    virtual void OnParentSizeChange( float width, float height ) override;

    size_t GetPageCount() const;

    CControl* GetPage( size_t index ) const;

private:
    void AddTabPage( CControl* pControl );

    void RemovePage( CControl* pControl );

    void UpdateTabButtonStyle();

    void CreateTabButton( );

    void DeleteTabButton( );

    void UpdateTabWindow();

    void UpdateTabButtonData();

    void UpdteButtonImage();
private:
    bool m_bShowText;
    size_t m_uTextSize;
    CVec2 m_vec2TabButtonSize;
    ETabButtonSideType m_eTabButtonSideType;
    ETabButtonPositionType m_eTabButtonPositionType;
    SReflectTextureInfo m_fragButtonImage;
    SReflectTextureInfo m_fragButtonToggedImage;
    std::vector< CControl* > m_windowVector;
    //self use
    std::map< CButton*, CLabel* > m_buttonLabelMap;
    size_t m_uCurrentIndex;
    static int nNum;
};

#endif