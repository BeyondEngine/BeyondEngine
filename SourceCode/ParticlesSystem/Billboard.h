#ifndef BEYOND_ENGINE_PARTICLESYSTEM_BILLBOARD_H__INCLUDE
#define BEYOND_ENGINE_PARTICLESYSTEM_BILLBOARD_H__INCLUDE


class CBillboard
{
public:
    CBillboard();
    ~CBillboard();

    void SetWidth( float width );
    void SetHeight( float height );

    void SetSize( float width, float height );

    void SetPosition( const kmVec3& position );
    const kmVec3& GetPosition() const;
    void GetPosition( kmVec3& position ) const;

    void SetUV( const CQuadT& uv );

    void SetColor( const CColor& color );

    void Roll( float radin );

    const CVertexPTC* GetVertex( );

    void SetTansform( const kmMat4* transform , bool bReset = true );

    void Reset();

private:
    void InitVertex();
    void InitVertexPosition();
    void UpdateVertex();
    void UpdateTransform();
private:
    CVertexPTC m_Vertexs[4];
    bool m_bNeedUpdate;
    float m_fWidth;
    float m_fHeight;
    float m_fRadin;
    kmVec3 m_vec3Position;
    kmMat4 m_mat4Transform;
};
#endif//BILLBOARD_H__INCLUDE

