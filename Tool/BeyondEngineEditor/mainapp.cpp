#include "stdafx.h"
#include "mainapp.h"
#include "FCEngineEditor.h"
#include "EngineSplash.h"
#include "wxPtrButtonEditor.h"

bool CEngineEditor::OnInit()
{
    bool ret = wxApp::OnInit();
    if ( ret )
    {
        CreatePtrEditor();
        m_pMainFrame = new CEditorMainFrame(wxT("FCEngineEditor"));
#ifndef _DEBUG
        wxBitmap bmp;
        bmp.LoadFile(wxT("../resource/splash.png"), wxBITMAP_TYPE_PNG);
        EngineSplash* pSplash = new EngineSplash(&bmp,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
            0, m_pMainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
        pSplash->Instance();
        BEATS_SAFE_DELETE(pSplash);
#endif
        m_pMainFrame->Show(true);
    }
    return ret;
}

wxPGEditor* CEngineEditor::GetPtrEditor() const
{
    return m_pPtrButtonEditor;
}

void CEngineEditor::CreatePtrEditor()
{
    m_pPtrButtonEditor = wxPropertyGrid::RegisterEditorClass(new wxPtrButtonEditor());;
}

CEditorMainFrame* CEngineEditor::GetMainFrame() const
{
    return m_pMainFrame;
}

IMPLEMENT_APP(CEngineEditor)