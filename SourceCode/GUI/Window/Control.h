#ifndef BEYOND_ENGINE_GUI_CONTROL_H__INCLUDE
#define BEYOND_ENGINE_GUI_CONTROL_H__INCLUDE

#include "Scene/Node.h"
#include "Action/ActionBase.h"
#include "Event/EventType.h"

#define DEFAULT_CONTROL_NAME _T("unname")

class CBaseEvent;
class CTouchEvent;
class CNodeAnimation;

class CControl : public CNode
{
    DECLARE_REFLECT_GUID( CControl, 0x1458a900, CNode )
public:
    enum ECornerPosition
    {
        eCP_TopLeft = 0,
        eCP_TopRight,
        eCP_BottomLeft,
        eCP_BottomRight
    };
public:
    CControl();
    virtual ~CControl();

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual void OnParentSizeChange( float width, float height );

    virtual bool OnTouchEvent( CTouchEvent* event ) override;

    virtual void Initialize() override;

    virtual void Uninitialize() override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual void SetName( const TString& strName ) override;

    virtual bool HitTest( float x, float y );

    virtual void SetParentNode( CNode* pParent ) override;

    virtual void DoRender() override;

    virtual void UpdateQuadP();
    const CQuadP& GetQuadP() const;

    void SetFocus( bool bFocus );
    bool IsFoucus() const;

    void SetColor( const CColor& color );
    const CColor& GetColor( bool bInherit = true ) const;
    bool IsInheritColorScale() const;

    void SetAnchor( const CVec2& anchor );
    void SetAbsolutePosition( const CVec2& position );
    const CVec2& GetAbsolutePosition() const;
    void SetPercentPosition( const CVec2& position );
    virtual void SetSize( const CVec2& size );
    void SetPercentSize( const CVec2& size );

    virtual const CVec2& GetRealSize() const;
    const CVec2& GetAnchor() const;

    void SetResponseClickEvent( bool bResponse );
    bool IsResponseClickEvent() const;

    bool IsEnable() const;
    void SetEnable( bool bEnable );

    size_t GetLayerID() const;
    void SetLayerId( size_t layerid );

    void SetUserData( void* pData);
    void* GetUserData() const;

    void SetEventDispatchToParent( bool bDispatch );

    void SetClip( bool bClip , const CRect& rect = CRect() );
    const CRect& GetClipRect() const;
    bool IsClip();

    void StopClickAnimation();

    void LocalToWorld(float &x, float &y);
    void LocalToParent(float &x, float &y);
    void WorldToLocal(float &x, float &y);
    void ParentToLocal(float &x, float &y);
    void SetRootFlag( bool bRoot );
    bool GetRootFlag() const;

    void SetAnimationName( const TString& animationName );
    void PressAnimationFinishCallBack( CBaseEvent* event );
    void ReleaseAnimationFinishCallBack( CBaseEvent* event );
    void GetCorner( ECornerPosition type, float& x, float& y );
#ifdef EDITOR_MODE
    enum EHitTestResult
    {
        eHTR_HIT_NONE,
        eHTR_HIT_CONTENT,
        eHTR_HIT_LEFT_CENTER,
        eHTR_HIT_RIGHT_CENTER,
        eHTR_HIT_TOP_CENTER,
        eHTR_HIT_BOTTOM_CENTER,
        eHTR_HIT_TOP_LEFT,
        eHTR_HIT_TOP_RIGHT,
        eHTR_HIT_BOTTOM_LEFT,
        eHTR_HIT_BOTTOM_RIGHT,
        eHTR_HIT_ANCHOR,
        eHTR_HIT_EDGE,
        eHTR_HIT_ROTATION_BAR
    };
    EHitTestResult GetHitResult( float x, float y );
    CVec2 CalcPosFromMouse(float x, float y) const;
    CVec2 CalcSizeFromMouse(float x, float y, EHitTestResult hitPos);
    bool m_bEditorSelect;
    void SetEditorSelect( bool bSelect );
    bool IsEditorSelect() const;
#endif // EDITOR_MODE

protected:
    virtual void OnTouchBegin( float x, float y );
    virtual void OnTouchMove( float x, float y );
    virtual void OnTouchEnded( float x, float y );
    virtual void OnTouchCancelled( float x, float y );
    virtual void OnTouchDoubleTapped( float x, float y );
    virtual void OnTouchSingleTapped( float x, float y );
    virtual void OnTouchScorll( float x, float y, int delta );
    virtual void OnTouchPinched( float x, float y );

    const TString& GetDefaultName() const;

    virtual void OnClickBegin();
    virtual void OnClickEnd();

    virtual void CalcRealSize();

    bool HandleTouchEvent( CTouchEvent* event );

    void GetAnchorPosition( float& x, float& y );

private:
    void DispatchEvent( EEventType type , float x, float y, int delta );
    void CalcInheritColor();
    void CalcRealPosition( CVec2& relPosition );
    CNodeAnimation* CreateNodeAnimation( const TString& animationName );
protected:
    CVec2 m_vec2Anchor;
    CVec2 m_vec2AbsolutePosition;
    CVec2 m_vec2PercentPosition;
    CVec2 m_vec2Size;
    CVec2 m_vec2PercentSize;
    CColor m_color;
    CColor m_colorInherit;
    CRect m_rectClip;

    bool m_bUseInheritColorScale;

    CVec2 m_vecRealSize;
    float m_fTargetWidth;
    float m_fTargetHeight;

    CQuadP m_quadp;

    TString m_strDefaultName;
    bool m_bEnable;
    bool m_bResponseClickEvent;
    bool m_bResiveTouchEvent;
    bool m_bTouchInRect;
    size_t m_uLayerID;
    TString m_strPressAnimationName;
    TString m_strReleaseAimationName;
private:
    bool m_bClick;
    bool m_bRelease;
    bool m_bIsClip;
    float m_fButtonScale;
    bool m_bTurnScale;
    bool m_bScaleAnimation;
    bool m_bDispatchEventToParent;
    bool m_bFoucs;
    bool m_bRoot;
    CActionBase* m_pClickAction;
    CActionBase* m_pClickedAction;
    CNodeAnimation* m_pPressAnimation;
    CNodeAnimation* m_pReleaseAnimation;
    void* m_pUserData;
    bool m_bUninitialize;
    static int iIndex;
};

#endif