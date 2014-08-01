#include "stdafx.h"
#include "EngineSplash.h"
#include "EditorMainFrame.h"
#include "TransparentText.h"
#include "BeyondEngineEditorComponentWindow.h"

BEGIN_EVENT_TABLE(CEngineSplash, wxSplashScreen)
    EVT_CLOSE(CEngineSplash::OnCloseWindow)
END_EVENT_TABLE()

CEngineSplash::CEngineSplash(wxBitmap& bitmap, long splashStyle, int milliseconds,
                           wxWindow* parent, wxWindowID id,
                           const wxPoint& pos ,
                           const wxSize& size ,
                           long style) :
                    wxSplashScreen(bitmap, splashStyle, milliseconds, parent, id, pos, size, style)
                        , m_bClosed(false)
{
}

CEngineSplash::~CEngineSplash()
{
}

void CEngineSplash::Instance()
{
    wxWindow *win = GetSplashWindow();
    wxStaticText *text = new CTransparentText( win,
        wxID_EXIT,
        wxT("BeyondEngineEditor V_0_1"),
        wxPoint(13, 11)
        );
    wxFont font = text->GetFont();
    font.SetPointSize(2 * font.GetPointSize());
    text->SetFont(font);
}

void CEngineSplash::OnCloseWindow( wxCloseEvent &/*event*/ )
{
    if (!m_bClosed) // I don't know why sometimes this will come in many times.
    {
        m_bClosed = true;
        CEditorMainFrame* pMainFrame = (CEditorMainFrame*)GetParent();
        pMainFrame->Show(true);
        pMainFrame->Thaw();
        pMainFrame->StartRenderUpdate();
        Destroy();
        // HACK: we can only do some initialize logic after the opengl is initialized.
        // We initialize OPENGL in m_pViewScreen->SetContextToCurrent() of Show() That's why we call PostInitialize here.
        pMainFrame->PostInitialize();
    }
}
