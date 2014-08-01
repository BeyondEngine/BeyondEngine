#include "stdafx.h"
#include "Billboard.h"

CBillboard::CBillboard()
    : m_bNeedUpdate(false)
    , m_fWidth(1.0f)
    , m_fHeight(1.0f)
    , m_fRadin(0.0f)
{
    kmVec3Fill( &m_vec3Position, 0.0f, 0.0f, 0.0f );
    kmMat4Identity( &m_mat4Transform );
    InitVertex();
}

CBillboard::~CBillboard()
{

}

void CBillboard::SetWidth( float width )
{
    m_fWidth = width;
    m_bNeedUpdate = true;
}

void CBillboard::SetHeight( float height )
{
    m_fHeight = height;
    m_bNeedUpdate = true;
}

void CBillboard::SetSize( float width, float height )
{
    m_fWidth = width;
    m_fHeight = height;
    m_bNeedUpdate = true;
}

void CBillboard::SetPosition( const kmVec3& position )
{
    kmVec3Fill( &m_vec3Position, position.x, position.y, position.z );
    m_bNeedUpdate = true;
}

void CBillboard::InitVertex()
{
    //vertex
    InitVertexPosition();
    //tl
    m_Vertexs[ 0 ].tex.u = 0.0f;
    m_Vertexs[ 0 ].tex.v = 0.0f;
    m_Vertexs[ 0 ].color = 0x000000ff;
    //bl
    m_Vertexs[ 1 ].tex.u = 0.0f;
    m_Vertexs[ 1 ].tex.v = 1.0f;
    m_Vertexs[ 1 ].color = 0x000000ff;
    //br
    m_Vertexs[ 2 ].tex.u = 1.0f;
    m_Vertexs[ 2 ].tex.v = 1.0f;
    m_Vertexs[ 2 ].color = 0x000000ff;
    //tr
    m_Vertexs[ 3 ].tex.u = 1.0f;
    m_Vertexs[ 3 ].tex.v = 0.0f;
    m_Vertexs[ 3 ].color = 0x000000ff;
}

void CBillboard::SetUV( const CQuadT& uv )
{
    //tl
    m_Vertexs[ 0 ].tex.u = uv.tl.u;
    m_Vertexs[ 0 ].tex.v = uv.tl.v;
    //bl
    m_Vertexs[ 1 ].tex.u = uv.bl.u;
    m_Vertexs[ 1 ].tex.v = uv.bl.v;
    //br
    m_Vertexs[ 2 ].tex.u = uv.br.u;
    m_Vertexs[ 2 ].tex.v = uv.br.v;
    //tr
    m_Vertexs[ 3 ].tex.u = uv.tr.u;
    m_Vertexs[ 3 ].tex.v = uv.tr.v;
}

void CBillboard::SetColor( const CColor& color )
{
    //tl
    m_Vertexs[ 0 ].color = color;
    //bl
    m_Vertexs[ 1 ].color = color;
    //br
    m_Vertexs[ 2 ].color = color;
    //tr
    m_Vertexs[ 3 ].color = color;
}

void CBillboard::UpdateVertex()
{
    InitVertexPosition();
    for ( auto i = 0; i < 4; ++i )
    {
        kmVec3Transform( &m_Vertexs[ i ].position,&m_Vertexs[ i ].position, &m_mat4Transform );
    }
}

const CVertexPTC* CBillboard::GetVertex()
{
    if ( m_bNeedUpdate )
    {
        UpdateTransform();
        UpdateVertex();
        m_bNeedUpdate = false;
    }
    return &m_Vertexs[0];
}

void CBillboard::UpdateTransform()
{
    kmMat4 rotateMat;
    kmMat4Identity( &rotateMat );
    kmMat4RotationPitchYawRoll( &rotateMat, 0, 0, m_fRadin );
    kmMat4 scaleMat;
    kmMat4Identity( &scaleMat );
    kmMat4Scaling( &scaleMat, m_fWidth, m_fHeight, 0 );

    kmMat4Identity( &m_mat4Transform );
    kmMat4Multiply( &m_mat4Transform, &rotateMat, &scaleMat );
}

void CBillboard::SetTansform( const kmMat4* transform , bool bReset )
{
    if ( bReset )
    {
        kmMat4Fill( &m_mat4Transform, transform->mat );
    }
    else
    {
        kmMat4Multiply( &m_mat4Transform, &m_mat4Transform, transform );
    }
    UpdateVertex();
    m_bNeedUpdate = false;
}

void CBillboard::InitVertexPosition()
{
    kmVec3Fill( &m_Vertexs[ 0 ].position, -0.5f, 0.5f, 0 );
    kmVec3Fill( &m_Vertexs[ 1 ].position, -0.5f, -0.5f, 0 );
    kmVec3Fill( &m_Vertexs[ 2 ].position, 0.5f , -0.5f, 0 );
    kmVec3Fill( &m_Vertexs[ 3 ].position, 0.5f, 0.5f, 0 );
}

void CBillboard::Reset()
{
    InitVertex();
}

const kmVec3& CBillboard::GetPosition() const
{
    return m_vec3Position;
}

void CBillboard::GetPosition( kmVec3& position ) const
{
    kmVec3Fill( &position, m_vec3Position.x, m_vec3Position.y, m_vec3Position.z );
}

void CBillboard::Roll( const float radin )
{
    m_fRadin = radin;
    m_bNeedUpdate = true;
}
