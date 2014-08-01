#ifndef BEYOND_ENGINE_GUI_SCROLLVIEW_H__INCLUDE
#define BEYOND_ENGINE_GUI_SCROLLVIEW_H__INCLUDE

#include "TextureControl.h"
#include "ScrollItem.h"

enum EScrollDeriction
{
    eSD_Left_Right = 0,
    eSD_Top_Bottom,
};

class CScrollView : public CTextureControl
{
    DECLARE_REFLECT_GUID( CScrollView, 0x1458a908, CTextureControl )
public:
    CScrollView();
    virtual ~CScrollView();

    virtual void Initialize() override;

    virtual void Uninitialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual bool OnDependencyChange( void* pComponentAddr, CComponentBase* pComponent ) override;

    virtual void UpdateQuadP() override;

    virtual void Update( float dtt ) override;

    virtual void OnTouchBegin( float x, float y ) override;

    virtual void OnTouchEnded(  float x, float y ) override;

    virtual void OnTouchMove( float x, float y ) override;

    virtual bool OnTouchEvent( CTouchEvent* event ) override;

    CScrollItem* GetScrollItem( size_t index );

    virtual void OnWorldTransformUpdate() override;

    void SetItemCount( size_t count );

    virtual void OnParentSizeChange( float width, float height ) override;

    void UpdateItemVisibility();

private:
    CScrollItem* CreateItem();
    void DeleteItem();
    void LayeroutItems();
    void Scroll( float distance );
    bool CheckBack();
    bool CheckFront();
    void SpringBack( float dtt );
    void UpdateScissorRect();

private:
    EScrollDeriction m_eScrollDerictionType;
    size_t m_uItemCount;
    float m_fBackForce;
    float m_fBackDistance;
    float m_fInertiaForce;
    float m_fItemWidth;
    float m_fItemOffset;
    float m_fItemSpace;
    size_t m_uBackCount;
    CScrollItem* m_pScrollItem;
    CScrollItem* m_pClickItem;

    std::vector< CScrollItem* > m_Items;
    bool m_bTouch;
    bool m_bBeginMove;
    float m_fMoveDelta;
    bool m_bSpringBack;
    bool m_bInertia;
    CVec2 m_vec2StartPoint;
    CVec2 m_vec2CurrentPoint;
    CVec2 m_vec2LastPoint;
    static int nNum;
};
#endif 
