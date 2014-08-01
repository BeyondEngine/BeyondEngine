#include "stdafx.h"
#include "BillBoardSet.h"
#include "Render/RenderGroup.h"
#include "Render/RenderGroupManager.h"
#include "Billboard.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Render/Camera.h"

CBillboardSet::CBillboardSet()
    : m_pMaterial(nullptr)
    , m_pRenderBatch(nullptr)
{
    kmMat4Identity( &m_mat4Uniform );
    m_eType = eBTFACE_CAMERA_TOTALLY;
    m_uniform.SetName(_T("u_transforMatrix"));
    m_uniform.SetType(eSUT_Matrix4f);
}

CBillboardSet::~CBillboardSet()
{
    BEATS_SAFE_DELETE( m_pRenderBatch );
}

void CBillboardSet::SetType( EBillboardType type )
{
    m_eType = type;
}

void CBillboardSet::DoRender()
{
    if ( m_pRenderBatch && m_pMaterial )
    {
        CalclationUniform();
        m_pRenderBatch->Clear();
        unsigned short pIndex[6] = {0,1,2,0,2,3};
        for ( size_t i = 0 ; i < m_vecBillboards.size(); ++i )
        {
            kmMat4 tanslatMat;
            kmMat4Identity( &tanslatMat );
            kmVec3 posotion;
            m_vecBillboards[ i ]->GetPosition( posotion );
            kmMat4 m;
            kmMat4Translation( &tanslatMat, posotion.x, posotion.y, posotion.z );
            kmMat4Multiply( &m, &tanslatMat, &m_mat4Uniform );
            const CVertexPTC* pBuffer = m_vecBillboards[i]->GetVertex();
            BEATS_ASSERT( nullptr != pBuffer );
            m_pRenderBatch->AddIndexedVertices( pBuffer, 4, pIndex, 6 , &m );
        }
        std::vector<float>& uniformData = m_uniform.GetData();
        uniformData.resize(16);
        for ( int i = 0; i < 16; ++i )
        {
            uniformData[i] = GetLocalTM().mat[i];
        }

        CRenderGroup *pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D);
        pRenderGroup->AddRenderBatch( m_pRenderBatch );
    }
}

void CBillboardSet::AddBillboard( CBillboard* pBillBoard )
{
    m_vecBillboards.push_back( pBillBoard );
}

void CBillboardSet::RemoveBillboard( CBillboard* pBillboard )
{
    auto iter = m_vecBillboards.begin();
    for ( ; iter != m_vecBillboards.end(); ++iter )
    {
        if ( *iter == pBillboard )
        {
            m_vecBillboards.erase( iter );
            break;
        }
    }
}

void CBillboardSet::SetMaterial( SharePtr<CMaterial> pMaterial )
{
    m_pMaterial = pMaterial;
    if ( pMaterial )
    {
        if ( nullptr == m_pRenderBatch )
        {
            m_pRenderBatch = new CRenderBatch( VERTEX_FORMAT(CVertexPTC), m_pMaterial, GL_TRIANGLES, true );
        }
        else
        {
            m_pRenderBatch->SetMaterial( pMaterial );
        }
        m_pMaterial->AddUniform(&m_uniform );
    }
}

void CBillboardSet::Clear()
{
    m_vecBillboards.clear();
}

void CBillboardSet::CalclationUniform()
{
    kmMat4Identity( &m_mat4Uniform );
    switch ( m_eType )
    {
    case eBTFACE_CAMERA_TOTALLY:
        CalclationUniformFaceCameraTotally( m_mat4Uniform );
        break;
    case eBTFACE_CAMERA:
        CalclationUniformFaceCamera( m_mat4Uniform );
        break;
    case eBTFACE_AXLEY:
        CalclationUniformFaceY( m_mat4Uniform );
        break;
    case eBTFACE_AXLEX:
        CalclationUniformFaceX( m_mat4Uniform );
        break;
    default:
        MatrixRotate( m_mat4Uniform );
        break;
    }
}

void CBillboardSet::CalclationUniformFaceCameraTotally( kmMat4& mat )
{
    CScene* pCurrentScene = CSceneManager::GetInstance()->GetCurrentScene();
    if ( pCurrentScene )
    {
        CCamera* pCamera = pCurrentScene->GetCamera( CCamera::eCT_3D );
        if ( pCamera )
        {
            mat = pCamera->GetViewMatrix();
            kmMat4 translateMatrix;
            kmMat4Translation( &translateMatrix, -mat.mat[ 12 ], -mat.mat[ 13 ], -mat.mat[ 14 ] );
            kmMat4Multiply( &mat, &translateMatrix, &mat );
            kmMat4Inverse( &mat, &mat );//finally

            MatrixRotate( mat );
        }
    }
}

void CBillboardSet::CalclationUniformFaceCamera( kmMat4& mat )
{
    CScene* pCurrentScene = CSceneManager::GetInstance()->GetCurrentScene();
    if ( pCurrentScene )
    {
        CCamera* pCamera = pCurrentScene->GetCamera( CCamera::eCT_3D );
        if ( pCamera )
        {
            mat = pCamera->GetViewMatrix();
            kmMat4 translateMatrix;
            kmMat4Translation( &translateMatrix, -mat.mat[ 12 ], -mat.mat[ 13 ], -mat.mat[ 14 ] );
            kmMat4Multiply( &mat, &translateMatrix, &mat );
            kmMat4Inverse( &mat, &mat );
            mat.mat[ 4 ] = 0.0f;
            mat.mat[ 5 ] = 1.0f;
            mat.mat[ 6 ] = 0.0f;

            MatrixRotate( mat );
        }
    }
}

void CBillboardSet::CalclationUniformFaceY( kmMat4& mat )
{
    kmMat4RotationPitchYawRoll( &mat, MATH_PI_HALF, 0.0f, 0.0f );
    MatrixRotate( mat );
}

void CBillboardSet::CalclationUniformFaceX( kmMat4& mat )
{
    kmMat4RotationPitchYawRoll( &mat, 0.0f, MATH_PI_HALF * -1.0f, 0.0f );
    MatrixRotate( mat );
}


void CBillboardSet::MatrixRotate( kmMat4& mat )
{
    kmMat3 originMat;
    kmMat4ExtractRotation( &originMat, &GetLocalTM() );//origin
    kmMat3Inverse( &originMat, 1.0f, &originMat );
    kmVec3 translate;
    kmVec3Fill( &translate, 0.0f, 0.0f, 0.0f );
    kmMat4 invertMat4;
    kmMat4RotationTranslation( &invertMat4, &originMat, &translate );
    kmMat4Multiply( &mat, &invertMat4 , &mat);
}

size_t CBillboardSet::GetBillboardCount() const
{
    return m_vecBillboards.size();
}

SharePtr<CMaterial> CBillboardSet::GetMaterial() const
{
    return m_pMaterial;
}

void CBillboardSet::SortBillBoards()
{
    BEATS_ASSERT( m_vecBillboards.size() != 0, _T("the billboard count must bigger than zero"));
    CScene* pScene = CSceneManager::GetInstance()->GetCurrentScene();
    if ( pScene )
    {
        CCamera* pCamera = pScene->GetCamera(CCamera::eCT_3D);
        if ( pCamera )
        {
            const kmVec3& cameraPosition = pCamera->GetViewPos();
            SortBillBoards( &m_vecBillboards[0] , 0, m_vecBillboards.size() - 1, cameraPosition );
        }
    }
}

void CBillboardSet::SortBillBoards( CBillboard** pBillBoard , int lowIndex, int highIndex , const kmVec3& cameraPosition)
{
    if ( lowIndex < highIndex )
    {
        CBillboard* pTempBillboard = pBillBoard[ lowIndex ];
        kmVec3 tempPosition;
        pTempBillboard->GetPosition( tempPosition );
        kmVec3 tempLengthVector;
        kmVec3Subtract( &tempLengthVector, &tempPosition, &cameraPosition );

        int i = lowIndex, j = highIndex;
        while ( i < j )
        {
            kmVec3 highPosition;
            pBillBoard[j]->GetPosition( highPosition );
            kmVec3 highLengthVector;
            kmVec3Subtract( &highLengthVector, &highPosition, &cameraPosition );
            while ( i < j && kmVec3Length( &highLengthVector ) < kmVec3Length( &tempLengthVector ))//right
            {
                j--;
                pBillBoard[j]->GetPosition( highPosition );
                kmVec3Subtract( &highLengthVector, &highPosition, &cameraPosition );
            }
            if ( i < j )
            {
                pBillBoard[ i++ ] = pBillBoard[ j ];
            }
            kmVec3 lowPosition;
            pBillBoard[ i ]->GetPosition( lowPosition );
            kmVec3 lowLengthVector;
            kmVec3Subtract( &lowLengthVector, &lowPosition, &cameraPosition );
            while ( i < j && kmVec3Length( &lowLengthVector ) >= kmVec3Length( &tempLengthVector ))//left
            {
                i++;
                pBillBoard[i]->GetPosition( lowPosition );
                kmVec3Subtract( &lowLengthVector, &lowPosition, &cameraPosition );
            }
            if ( i < j )
            {
                pBillBoard[ j-- ] = pBillBoard[ i ];
            }
        }
        pBillBoard[ i ] = pTempBillboard;

        SortBillBoards( pBillBoard, lowIndex, i -1, cameraPosition );
        SortBillBoards( pBillBoard, i + 1, highIndex, cameraPosition );
    }
}