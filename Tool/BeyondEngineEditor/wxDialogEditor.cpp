#include "stdafx.h"
#include "wxDialogEditor.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"
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
#include "GradientCursor.h"
#include "GradientCtrl.h"
#include "RandomPropertyDialog.h"

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
    AddDeleteButton(property, buttons);

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
            if (ctrl->GetLabel().CmpNoCase(_T("-")) == 0)
            {
                DeleteEvent(propGrid, property);
            }
            else
            {
                CTexturePreviewDialog* pDialog = (CTexturePreviewDialog*)m_pDialog;
                BEATS_ASSERT(pDialog != nullptr);
                CTexturePropertyDescription* pTexturePropertyDescription = static_cast<CTexturePropertyDescription*>(pPropertyDescription);
                TString* pValue = (TString*)pTexturePropertyDescription->GetValue(eVT_CurrentValue);
                if (!pValue->empty())
                {
                    std::vector<TString> strData;
                    CStringHelper::GetInstance()->SplitString(pValue->c_str(), _T("@"), strData);
                    BEATS_ASSERT(strData.size() == 2);
                    const STexturePreviewInfo* pInfo = pDialog->GetTextureInfo(strData[0].c_str(), strData[1].c_str());
                    if (pInfo == NULL)
                    {
                        pDialog->SetCurrentImage(_T(""), _T(""));
                    }
                    else
                    {
                        pDialog->SetCurrentImage(pInfo->fileName, pInfo->textureName);
                    }
                }
                if (pDialog->ShowModal() == wxID_OK)
                {
                    char szTmp[1024];
                    pTexturePropertyDescription->GetValueAsChar(eVT_CurrentValue, szTmp);
                    wxString textureInfo = szTmp;
                    wxString newInfo;
                    int nInfoIndex = pDialog->GetCurrentIndex();
                    if (nInfoIndex != INVALID_DATA)
                    {
                        const STexturePreviewInfo* info = pDialog->GetTextureInfo(nInfoIndex);
                        BEATS_ASSERT(info != nullptr);
                        newInfo = info->fileName + _T("@") + info->textureName;
                    }
                    if (textureInfo != newInfo)
                    {
                        wxVariant newValue(newInfo);
                        pTexturePropertyDescription->SetValue(newValue, false);
                        property->SetValue(newValue);
                        propGrid->Refresh();
                    }
                }
            }
        }
        else if (pPropertyDescription->GetType() == eRPT_GradientColor)
        {
            CGradientColorPropertyDescription* pGradientColorPropertyDescription = static_cast<CGradientColorPropertyDescription*>(pPropertyDescription);
            pGradientColorPropertyDescription->ApplyToDialog();
            CEngineEditor* pEngineEditor = down_cast<CEngineEditor*>(wxApp::GetInstance());
            CGradientDialog* pDialog = pEngineEditor->GetGradientDialog();
            CGradientCtrl* pGradientCtrl = pDialog->GetGradientCtrl();
            std::map<float, CColor>& colorMap = pGradientColorPropertyDescription->GetColorMap();
            std::map<float, uint8_t>& alphaMap = pGradientColorPropertyDescription->GetAlphaMap();
            BEATS_ASSERT(pDialog != nullptr);
            int nRet = pDialog->ShowModal();
            if (nRet == wxYES)
            {
                const std::vector<CGradientCursor*>& colorList = pGradientCtrl->GetColorList();
                colorMap.clear();
                for (uint32_t i = 0; i < colorList.size(); ++i)
                {
                    float fPos = colorList[i]->GetPosPercent();
                    wxColor color = colorList[i]->GetColor();
                    colorMap[fPos] = CColor(color.Red(), color.Green(), color.Blue(), color.Alpha());
                }

                const std::vector<CGradientCursor*>& alphaList = pGradientCtrl->GetAlphaList();
                alphaMap.clear();
                for (uint32_t i = 0; i < alphaList.size(); ++i)
                {
                    float fPos = alphaList[i]->GetPosPercent();
                    uint8_t alpha = alphaList[i]->GetColor().Red();
                    alphaMap[fPos] = alpha;
                }
                pGradientColorPropertyDescription->SetValueImage(pGradientCtrl->GetImage());
                wxString strNewValue = pGradientColorPropertyDescription->WriteToString();
                wxVariant newValue(strNewValue);
                pGradientColorPropertyDescription->SetValue(newValue, false);
                wxBitmap bitmap(pGradientColorPropertyDescription->GetValueImage());
                property->SetValueImage(bitmap);
                property->RecreateEditor();
                propGrid->Refresh();
            }
        }
        else if (pPropertyDescription->GetType() == eRPT_List)
        {
            CPropertyGridEditor* pDialog = down_cast<CPropertyGridEditor*>(m_pDialog);
            BEATS_ASSERT(pDialog != nullptr);
            pDialog->SetProperty(property);
            pDialog->ShowModal();
        }
        else if (pPropertyDescription->GetType() == eRPT_RandomValue)
        {
            CRandomPropertyDialog* pDialog = down_cast<CRandomPropertyDialog*>(m_pDialog);
            SRandomValue* pRandomValue = (SRandomValue*)pPropertyDescription->GetValue(eVT_CurrentValue);
            pDialog->SetRandomValue(pRandomValue);
            wxPoint pos = wxGetMousePosition();
            pos.x -= m_pDialog->GetSize().x;
            m_pDialog->SetPosition(pos);
            m_pDialog->ShowModal();
            TCHAR szBuffer[MAX_PATH];
            pPropertyDescription->GetValueAsChar(eVT_CurrentValue, szBuffer);
            TString strDisplayString = szBuffer;
            size_t uPos = strDisplayString.find_first_of(',');
            if (uPos != std::string::npos)
            {
                strDisplayString = strDisplayString.substr(uPos + 1); // Skip the type string.
            }
            property->SetValue(strDisplayString);
            wxVariant newValue(pRandomValue);
            pPropertyDescription->SetValue(newValue, false);
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
