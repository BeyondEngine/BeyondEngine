#include "stdafx.h"
#include "ParticleControlWnd.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EditorSceneWindow.h"
#include "wx/valnum.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "wx/msw/private.h"
#include "ParticleSystem/ParticleManager.h"

CParticleControlWnd::CParticleControlWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name/* = wxDialogNameStr*/)
: super(parent, id, title, pos, size, style, name)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);

    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pPlayBtn = new wxButton(this, wxID_ANY, "Pause");
    m_pPlayBtn->SetToolTip("Press ctrl button to reset the particle emitter's random seed");
    pBtnSizer->Add(m_pPlayBtn, 1, wxALL, 1);
    wxButton* pStopButton = new wxButton(this, wxID_ANY, "Stop");
    pBtnSizer->Add(pStopButton, 1, wxALL, 1);
    pMainSizer->Add(pBtnSizer, 1, wxALL | wxEXPAND, 1);

    wxBoxSizer* pPlaybackSpeedSizer = new wxBoxSizer(wxHORIZONTAL);
    pPlaybackSpeedSizer->Add(new wxStaticText(this, wxID_ANY, "Playback Speed"), 1, wxALIGN_CENTRE_VERTICAL | wxLEFT, 1);
    m_pPlaybackSpeedTextCtrl = new wxTextCtrl(this, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<float>());
    pPlaybackSpeedSizer->Add(m_pPlaybackSpeedTextCtrl, 1, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 1);
    pMainSizer->Add(pPlaybackSpeedSizer, 1, wxALL | wxEXPAND, 0);

    wxBoxSizer* pPlaybackTimeSizer = new wxBoxSizer(wxHORIZONTAL);
    pPlaybackTimeSizer->Add(new wxStaticText(this, wxID_ANY, "Playback Time"), 1, wxALIGN_CENTRE_VERTICAL | wxLEFT, 1);
    m_pPlaybackTimeTextCtrl = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<float>());
    pPlaybackTimeSizer->Add(m_pPlaybackTimeTextCtrl, 1, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 1);
    pMainSizer->Add(pPlaybackTimeSizer, 1, wxALL | wxEXPAND, 0);

    pMainSizer->Fit(this);
    m_pPlayBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CParticleControlWnd::OnPlayBtnClicked), nullptr, this);
    pStopButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CParticleControlWnd::OnStopBtnClicked), nullptr, this);
}

CParticleControlWnd::~CParticleControlWnd()
{
}

void CParticleControlWnd::AttachEmitter(CParticleEmitter* pEmitter)
{
    if (m_pAttachedEmitter != pEmitter)
    {
        if (m_pAttachedEmitter != nullptr)
        {
            m_pAttachedEmitter->m_bRenderShape = false;
        }
        m_pAttachedEmitter = pEmitter;
        if (m_pAttachedEmitter != nullptr)
        {
            m_pAttachedEmitter->m_bRenderShape = true;
        }
    }
}

bool CParticleControlWnd::Show(bool bShow)
{
    if (bShow)
    {
        CEditorMainFrame* pMainFrame = ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame();
        wxPoint pt = pMainFrame->GetSceneWindow()->GetScreenPosition();
        wxSize sz = pMainFrame->GetSceneWindow()->GetSize();
        wxPoint pos = pt + sz;
        pos.x -= GetSize().x;
        pos.y -= GetSize().y;
        pos.y += 40; // HACK: Hard code, or find a better solution.
        pos = GetParent()->ScreenToClient(pos);
        SetPosition(pos);
    }
    return super::Show(bShow);
}

void CParticleControlWnd::Update(float /*dtt*/)
{
    super::Update();
    if (IsShown())
    {
        double fSpeed = 1.0f;
        m_pPlaybackSpeedTextCtrl->GetValue().ToDouble(&fSpeed);
        BEATS_ASSERT(m_pAttachedEmitter != nullptr);
        if (!m_pAttachedEmitter->IsPlaying() && !m_pAttachedEmitter->IsPaused())
        {
            CParticleManager::GetInstance()->RemovePlayingEmitter(m_pAttachedEmitter);
        }
        m_pAttachedEmitter->m_fPlaySpeed = fSpeed;
        m_pPlaybackTimeTextCtrl->SetValue(wxString::FromDouble(m_pAttachedEmitter->GetPlayingTime(true), 2));
        m_pPlayBtn->SetLabel(m_pAttachedEmitter->IsPlaying() ? "Pause" : "Simulate");
    }
}

void CParticleControlWnd::OnStopBtnClicked(wxCommandEvent& /*event*/)
{
    BEATS_ASSERT(m_pAttachedEmitter != nullptr);
    m_pAttachedEmitter->Stop();
}

void CParticleControlWnd::OnPlayBtnClicked(wxCommandEvent& /*event*/)
{
    BEATS_ASSERT(m_pAttachedEmitter != nullptr);
    bool bIsEmitterNodeDriven = m_pAttachedEmitter->GetParentNode() != nullptr;
    if (!bIsEmitterNodeDriven)
    {
        if (wxIsCtrlDown())
        {
            m_pAttachedEmitter->ResetRandomSeed();
        }
        m_pAttachedEmitter->IsPlaying() ? m_pAttachedEmitter->Pause() : m_pAttachedEmitter->Play();
        if (m_pAttachedEmitter->GetParentEmitter() != nullptr)
        {
            CParticleManager::GetInstance()->AddPlayingEmitter(m_pAttachedEmitter);
        }
    }
}
