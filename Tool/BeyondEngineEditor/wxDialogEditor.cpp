#include "stdafx.h"
#include "wxDialogEditor.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "PtrPropertyDescription.h"
#include "ListPropertyDescription.h"
#include "MapPropertyDescription.h"
#include "EditorMainFrame.h"
#include "EnginePropertyGrid.h"
#include "TexturePreviewDialog.h"
#include "TexturePropertyDescription.h"
#include "EngineEditor.h"
#include "GradientDialog.h"
#include "GradientColorPropertyDescription.h"
#include "PropertyGridEditor.h"

IMPLEMENT_DYNAMIC_CLASS(wxDialogEditor, wxPGTextCtrlEditor);
wxDialogEditor::wxDialogEditor()
    : m_pDialog(nullptr)
{
}

wxDialogEditor::wxDialogEditor(wxDialog* pDialog)
{
    BEATS_ASSERT(pDialog);
    m_pDialog = pDialog;
}

wxDialogEditor::~wxDialogEditor()
{
    if (m_pDialog->GetId() == wxID_ANY)
    {
        m_pDialog->Destroy();
    }
}

wxPGWindowList wxDialogEditor::CreateControls( wxPropertyGrid* propGrid,
                                                 wxPGProperty* property,
                                                 const wxPoint& pos,
                                                 const wxSize& sz ) const
{
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );
    buttons->Add(_T("..."));
    buttons->Finalize(propGrid, pos);

    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize() );
    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxDialogEditor::OnEvent( wxPropertyGrid* propGrid,
                                wxPGProperty* property,
                                wxWindow* ctrl,
                                wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_BUTTON )
    {
        CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(property->GetClientData());
        if (pPropertyDescription->GetType() == eRPT_Texture)
        {
            CTexturePreviewDialog* pDialog = (CTexturePreviewDialog*)m_pDialog;
            CTexturePropertyDescription* pTexturePropertyDescription = static_cast<CTexturePropertyDescription*>(pPropertyDescription);
            BEATS_ASSERT(pDialog != nullptr);
            int nInfoIndex = pDialog->ShowModal();
            if (nInfoIndex != INVALID_DATA)
            {
                TexturePreviewInfo info = pDialog->GetTextureInfo(nInfoIndex);
                bool bValueChanged = false;
                char szTmp[1024];
                pTexturePropertyDescription->GetValueAsChar(eVT_CurrentValue, szTmp);
                wxString textureInfo = szTmp;
                wxString newInfo = info.fileName + _T("@") + info.textureName;
                if (textureInfo != newInfo)
                {
                    bValueChanged = true;
                }
                if (bValueChanged)
                {
                    wxVariant newValue(newInfo);
                    pTexturePropertyDescription->SetValue(newValue, false);
                    property->SetValue(newValue);
                    propGrid->Refresh();
                }
            }
        }
        else if (pPropertyDescription->GetType() == eRPT_GradientColor)
        {
            CGradientDialog* pDialog = (CGradientDialog*)m_pDialog;
            CGradientColorPropertyDescription* pGradientColorPropertyDescription = static_cast<CGradientColorPropertyDescription*>(pPropertyDescription);
            BEATS_ASSERT(pDialog != nullptr);
            int nInfoIndex = pDialog->ShowModal();
            if (nInfoIndex != INVALID_DATA)
            {
                CColorSpline* pColorSpline = pGradientColorPropertyDescription->GetColorSpline();
                BEATS_ASSERT(pColorSpline);
                pColorSpline->m_mapColors.clear();
                int nPosCount = pDialog->GetPosCount();
                wxString strValue;
                for (int i = 0; i < nPosCount; i++)
                {
                    wxColor color = pDialog->GetColorByIndex(i);
                    float fPos = pDialog->GetPosByIndex(i);
                    pColorSpline->m_mapColors[fPos] = CColor(color.GetRGBA());
                }
                wxVariant newValue(wxString(_T("...")));
                pGradientColorPropertyDescription->SetValue(newValue, false);
                propGrid->ChangePropertyValue(property, newValue);
                propGrid->Refresh();
            }
        }
        else if (pPropertyDescription->GetType() == eRPT_List)
        {
            CPropertyGridEditor* pDialog = down_cast<CPropertyGridEditor*>(m_pDialog);
            BEATS_ASSERT(pDialog != nullptr);
            CListPropertyDescription* pPropertyDescription = static_cast<CListPropertyDescription*>(property->GetClientData());
            BEATS_ASSERT(pPropertyDescription != NULL);
            (void)pPropertyDescription;
            pDialog->SetProperty(property);
            pDialog->ShowModal();
        }
    }

    return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
}

void wxDialogEditor::SetDialog(wxDialog* pDialog)
{
    BEATS_ASSERT(pDialog != nullptr);
    m_pDialog = pDialog;
}

wxString wxDialogEditor::GetName() const
{
    return m_pDialog->GetLabel();
}
