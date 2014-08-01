#include "stdafx.h"
#include "ConstantCurveEditor.h"
#include "ConstantCurveProperty.h"

CConstantCurveEditor::CConstantCurveEditor()
{
}

CConstantCurveEditor::~CConstantCurveEditor()
{
}


bool CConstantCurveEditor::GetPropertyCtrlValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl )
{
    bool bRes = true;
    wxTextCtrl* pTextCtrl = wxStaticCast(ctrl, wxTextCtrl);
    wxString textVal = pTextCtrl->GetValue();
    CConstantCurveProperty* p = (CConstantCurveProperty*)property;
    if (p->GetValueType() == CConstantCurveProperty::ConstantRandom_Type)
    {
        double iValue1 = p->GetConstantProperty1()->GetValue().GetDouble();
        double iValue2 = p->GetConstantProperty2()->GetValue().GetDouble();
        p->SetConstant1(iValue1);
        p->SetConstant2(iValue2);
    }
    if ( property->UsesAutoUnspecified() && textVal.empty() )
    {
        variant.MakeNull();
        bRes = true;
    }
    else
    {
        bRes = property->StringToValue(variant, textVal, wxPG_EDITABLE_VALUE);
        if ( !bRes && variant.IsNull() )
            bRes = true;
    }
    
    return bRes;
}

bool CConstantCurveEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const
{
    return GetTextCtrlValueFromControl(variant, property, ctrl);
}

void CConstantCurveEditor::DrawValue(wxDC &dc, const wxRect &rect, wxPGProperty *property, const wxString &text) const
{
    CConstantCurveProperty* p = (CConstantCurveProperty*)property;
    wxSize drawSize;
    drawSize.x = rect.x + wxPG_XBEFORETEXT - DEFAULT_IMAGE_OFFSET_INCREMENT;
    drawSize.y = rect.y;
    if (p->GetValueType() == CConstantCurveProperty::Constant_Type)
    {
        dc.DrawText( text, drawSize.x, drawSize.y );
    }
    else if (p->GetValueType() == CConstantCurveProperty::ConstantRandom_Type)
    {
        int iX1 = drawSize.x;
        int iX2 = drawSize.x + (drawSize.GetWidth() - DEFAULT_PGBUTTONWIDTH)*0.5;
        int iY = drawSize.y;
        dc.DrawLine(iX2 - wxPG_XBEFORETEXT, iY, iX2 - wxPG_XBEFORETEXT, iY + drawSize.GetHeight());
        dc.DrawText( p->GetConstantProperty1()->GetValue().GetString(), iX1, iY );
        dc.DrawText( p->GetConstantProperty2()->GetValue().GetString(), iX2, iY );
    }
}

wxPGWindowList CConstantCurveEditor::CreateControls( wxPropertyGrid* pPropGrid,
                                                           wxPGProperty* pProperty,
                                                           const wxPoint& pos,
                                                           const wxSize& sz ) const
{
    wxWindow* pWnd1 = NULL;
    wxWindow* pWnd2 = pPropGrid->GenerateEditorButton(pos,sz);
    wxPGWindowList list;
    CConstantCurveProperty* p = (CConstantCurveProperty*)pProperty;
    p->SetValueImageSizeX(sz.x);
    p->HideProperty(true, true);
    if (p->GetValueType() == CConstantCurveProperty::Constant_Type)
    {
        wxString text;
        if ( !pProperty->IsValueUnspecified() )
            text = pProperty->GetValueAsString(pProperty->HasFlag(wxPG_PROP_READONLY)?0:wxPG_EDITABLE_VALUE);
        pWnd1 = pPropGrid->GenerateEditorTextCtrl(pos, sz, text, pWnd2, pProperty->GetMaxLength());
    }
    else if(p->GetValueType() == CConstantCurveProperty::Curve_Type)
    {
        
    }
    else if(p->GetValueType() == CConstantCurveProperty::ConstantRandom_Type)
    {
        p->HideProperty(false, false);
    }
    else if(p->GetValueType() == CConstantCurveProperty::CurveRandom_Type)
    {

    }
    list = wxPGWindowList(pWnd1, pWnd2);
    return list;
}
