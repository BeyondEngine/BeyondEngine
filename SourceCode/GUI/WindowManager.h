#ifndef BEYOND_ENGINE_GUI_WINDOWMANAGER_H__INCLUDE
#define BEYOND_ENGINE_GUI_WINDOWMANAGER_H__INCLUDE

#define GUI_LAYER_ID 128
#define GRID9_VERTICES_COUNT 16
#define GRID9_INDICES_COUNT 54

#include "Window/Control.h"

//forward declaration
class CTextureControl;
class CBaseEvent;
class CTouchEvent;
class CMaterial;
class CTextureFrag;
class CTopWindow;
class CShaderUniform;
class CTexture;

class CWindowManager
{
    BEATS_DECLARE_SINGLETON(CWindowManager);
    typedef std::map<TString, CControl*> TControlMap;
    typedef std::function<void(CBaseEvent*)> EventHandler;
    typedef std::vector< EventHandler > TRecipientVector;
    typedef std::map< CControl*, TRecipientVector > TEventRecipientMap;
    typedef std::vector< CControl* > TControlVector;
    typedef std::vector< CTopWindow* > TTopWindowVector;
    typedef std::map< TString, CTopWindow* > TTopWindowNameMap;

public:

    template<typename T>
    T* CreateControl( const TString& name );

    template<typename T>
    T* CreateControl( const TString& name, CControl* pParent );

    void AddToRoot( CControl* pControl );

    void AttachToNode( CControl* pControl );

    void RemoveFromRoot( CControl* pControl );

    void DetachToNode( CControl* pControl );

    void RegisterControl( CControl* pControl );
    void UnregisterControl(CControl* pControl);

    void LogoutControl( CControl* pControl );

    CControl* GetControl( const TString& name );
    CControl* GetControl( float x, float y );

    bool DestoryControl( CControl* pControl );

    template<typename FUNC, typename ObjectType>
    void RegisterEventRecipient( CControl* pControl, const FUNC& func, ObjectType* pObj );

    template< typename ObjectType>
    void LogoutEventRecipient( CControl* pControl, ObjectType* pObj );

    void DispatchEventToRecipient( CControl* pControl, CBaseEvent* pEvent );

    void Update( float dtt );

    void Render();

    void OnSizeChange();

    void OnTouchEvent( CTouchEvent* event );

    void OnCharEvent( const TCHAR* data, int num );

    void RenderBaseControl( const CQuadP* quadp, const CColor& color , bool isClip , const CRect& rect = CRect(), const kmMat4* pTransform = NULL );
    void RenderTextureControl( const CTextureFrag* pFrag, bool bGrid9, const CVec2& grid9Offset, const CQuadP* quadp, const CQuadT* quadt, const CColor& color, 
        bool isClip, bool bDisable = false, const CRect& rect = CRect(), const kmMat4* pTransform = NULL );
    void RenderEditorSelect( const CQuadP* quadp , const kmMat4& transform );

    void Initialize();

    void RegisterTopWindow( CTopWindow* pTopWindow );
    void UnregisterTopWindow( CTopWindow* pTopWindow );
    void LogoutTopWindow( CTopWindow* pTopWindow );
    void AddToRootTopWindow( CTopWindow* pTopWindow );
    void RemoveFromRootTopWindow( CTopWindow* pTopWindow );
    CTopWindow* GetTopWindowByName( const TString& name );
    void SetActiveTopWindow( const TString& name );
    void SetActiveTopWindow( CTopWindow* pTopWindow );

    void SetFocusControl( CControl* pControl );
private:
    SharePtr<CMaterial> GetScissorMaterial( GLuint texID, const CRect& rect );
    SharePtr<CMaterial> GetTextrueMateial( GLuint id );
    SharePtr<CMaterial> GetDefaultMaterial();
    SharePtr<CMaterial> GetBaseMaterial();
    SharePtr<CMaterial> GetColorMaterial();
    SharePtr<CMaterial> GetEditorMaterial();
    SharePtr<CMaterial> CreateScissorMaterial( const CRect& rect );
    void InitFontFace();
    void SetGrid9VerticesAndIndices( const CVec2& grid9Offset, const CQuadP* quadp, const CQuadT* quadt, 
        CVertexPTC (&vertices)[GRID9_VERTICES_COUNT], unsigned short (&indices)[GRID9_INDICES_COUNT],
        const CColor& color) const;
    CControl* RecursiveChildren( CControl* pControl , float x, float y );
private:
    TEventRecipientMap m_mapRecipient;
    TControlMap m_mapControl;
    TControlVector m_rootControl;
    TControlVector m_vecControlByUserCreate;
    TControlVector m_vecControlAddNode;
    TTopWindowNameMap m_mapNameTopWindow;
    TTopWindowVector m_topWindowList;
    CTopWindow* m_pActiveTopWindow;
    SharePtr<CMaterial> m_pDefaultMateial;
    typedef std::map< size_t, SharePtr<CMaterial> > TScissorMaterial;
    std::map< GLuint, SharePtr<CMaterial> > m_mapTextureMaterial;
    std::map< GLuint, TScissorMaterial > m_mapScissorMaterial;
    std::vector< CRect > m_scissorRect;
    SharePtr<CMaterial> m_pBaseMaterial;
    SharePtr<CMaterial> m_pColorMaterial;
    SharePtr<CMaterial> m_pEditorMaterial;

    CShaderUniform* m_pColorMelody;
    CControl* m_pFocusControl;
};

template< typename ObjectType>
void CWindowManager::LogoutEventRecipient( CControl* pControl, ObjectType* pObj )
{
    TRecipientVector& recipientVector = m_mapRecipient[ pControl ];
    auto iter = recipientVector.begin();
    for ( ; iter != recipientVector.end(); ++iter )
    {
        if ( iter->target<ObjectType>() == pObj )
        {
            recipientVector.erase( iter );
            break;
        }
    }
}

template<typename FUNC, typename ObjectType>
void CWindowManager::RegisterEventRecipient( CControl* pControl, const FUNC& func , ObjectType* pObj )
{
    EventHandler handler = std::bind(func, pObj, std::placeholders::_1);
    m_mapRecipient[ pControl ].push_back( handler );
}

template<typename T>
T* CWindowManager::CreateControl( const TString& name, CControl* pParent )
{
    BEATS_ASSERT( pParent, _T("the parent can't be null") );
    T* control = new T( );
    control->Initialize();
    if ( name.compare(DEFAULT_CONTROL_NAME) != 0 )
    {
        control->SetName( name );
    }
    pParent->AddChild( control );
    m_vecControlByUserCreate.push_back( control );
    return control;
}

template<typename T>
T* CWindowManager::CreateControl( const TString& name )
{
    T* control = new T( );
    control->Initialize();
    if ( name.compare(DEFAULT_CONTROL_NAME) != 0 )
    {
        control->SetName( name );
    }
    m_vecControlByUserCreate.push_back( control );
    return control;
}

#endif