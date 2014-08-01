
#include "stdafx.h"
#include "ConstantCurveProperty.h"
#define X_UNNULL_SIZE wxSize(1,0);

CConstantCurveProperty::CConstantCurveProperty(const wxString& label, const wxString& name, double value)
    : CSingleTypeChoicePropertyBase(label, name, value)
    , m_dConstantValue_1(0.0)
    , m_dConstantValue_2(0.0)
    , m_pConstantProperty1(NULL)
    , m_pConstantProperty2(NULL)
    , m_bChildAppended(false)
    , m_pCurveBitmap(NULL)
{
    CreatConstantProperty();
}

CConstantCurveProperty::~CConstantCurveProperty()
{
    BEATS_SAFE_DELETE (m_pCurveBitmap);
}

wxSize CConstantCurveProperty::OnMeasureImage( int /*item*/ ) const
{   
    return X_UNNULL_SIZE;
}

void CConstantCurveProperty::SetCurveBitmap(wxBitmap& bitmap)
{
    BEATS_SAFE_DELETE (m_pCurveBitmap);
    
    m_pCurveBitmap = new wxBitmap(bitmap);
}

wxBitmap* CConstantCurveProperty::GetCurveBitmap() const
{
    return m_pCurveBitmap;
}

void CConstantCurveProperty::SetValueType(int iType)
{
    m_iType = iType;
}

int CConstantCurveProperty::GetValueType() const
{
    return m_iType;
}

void CConstantCurveProperty::SetConstant1(double Value1)
{
    m_dConstantValue_1 = Value1;
}

void CConstantCurveProperty::SetConstant2(double Value2)
{
    m_dConstantValue_2 = Value2;
}

double CConstantCurveProperty::GetConstant1() const
{
    return m_dConstantValue_1;
}

double CConstantCurveProperty::GetConstant2() const
{
    return m_dConstantValue_2;
}

wxPGProperty* CConstantCurveProperty::GetConstantProperty1() const
{
    return m_pConstantProperty1;
}
wxPGProperty* CConstantCurveProperty::GetConstantProperty2() const
{
    return m_pConstantProperty2;
}

void CConstantCurveProperty::CreatConstantProperty()
{
    m_pConstantProperty1 = new wxFloatProperty( wxT("Constant1"), wxPG_LABEL, m_dConstantValue_1);
    m_pConstantProperty2 = new wxFloatProperty( wxT("Constant2"), wxPG_LABEL, m_dConstantValue_2);
    AppendConstantProperty();
    HideProperty();
}

void CConstantCurveProperty::ReleaseConstantProperty()
{
    GetGrid()->DeleteProperty(m_pConstantProperty1);
    GetGrid()->DeleteProperty(m_pConstantProperty2);
}

void CConstantCurveProperty::HideProperty(bool bHide1,bool bHide2)
{
    m_pConstantProperty1->Hide(bHide1);
    m_pConstantProperty2->Hide(bHide2);
}

void CConstantCurveProperty::AppendConstantProperty()
{
    if (!m_bChildAppended)
    {
        AppendChild(m_pConstantProperty1);
        AppendChild(m_pConstantProperty2);
        m_bChildAppended = true;
    }
}

bool CConstantCurveProperty::StringToValue( wxVariant& variant, const wxString& text, int argFlags ) const
{
    return wxFloatProperty::StringToValue( variant, text, argFlags);
}

void CConstantCurveProperty::OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& /*paintdata*/)
{
    if(GetValueType() == CConstantCurveProperty::Curve_Type)
    {
        wxBitmap* pBmp = GetCurveBitmap();
        if (pBmp != NULL)
        {   
            wxBitmap* bmpNew = NULL;
            if (pBmp->GetWidth() != rect.x)
            {
                wxSize maxSz = GetGrid()->GetImageSize();
                wxSize imSz(pBmp->GetWidth(),pBmp->GetHeight());
                wxImage img = pBmp->ConvertToImage();
                img.Rescale(m_iImageSizeX, maxSz.y, wxIMAGE_QUALITY_HIGH);
                bmpNew = new wxBitmap(img, DEFAULT_DEPTH);
            }
            wxCHECK_RET( pBmp && pBmp->IsOk(), wxT("invalid bitmap") );
            wxCHECK_RET( rect.x >= 0, wxT("unexpected measure call") );
            dc.DrawBitmap(*bmpNew, rect.x - DEFAULT_IMAGE_OFFSET_INCREMENT, rect.y);
            BEATS_SAFE_DELETE(bmpNew);
        }
    }
    else if(GetValueType() == CConstantCurveProperty::ConstantRandom_Type)
    {
        
    }
}

void CConstantCurveProperty::SetValueImageSizeX(int iSizeX)
{
    m_iImageSizeX = iSizeX;
}