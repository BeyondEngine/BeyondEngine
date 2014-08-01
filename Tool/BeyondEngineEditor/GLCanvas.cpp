#include "stdafx.h"
#include "GL/glew.h"
#include "glfw3.h"
#include "GLCanvas.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include <mmsystem.h>
#include "PublicDef.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Spline/Curve.h"

#include "Render/RenderManager.h"
#include "Render/RenderObjectManager.h"
#include "Render/AnimationManager.h"
#include "Render/RenderPublic.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Render/SpriteFrame.h"
#include "Render/SpriteAnimation.h"
#include "Render/Sprite.h"
#include "Render/AnimationController.h"
#include "Render/Camera.h"

#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"

BEGIN_EVENT_TABLE(GLAnimationCanvas, wxGLCanvas)
    EVT_SIZE(GLAnimationCanvas::OnSize)
    EVT_PAINT(GLAnimationCanvas::OnPaint)
    EVT_KEY_DOWN(GLAnimationCanvas::OnKeyDown)
    EVT_KEY_UP(GLAnimationCanvas::OnKeyUp)
    EVT_MOUSE_EVENTS(GLAnimationCanvas::OnMouse)
    EVT_MOUSE_CAPTURE_LOST(GLAnimationCanvas::MouseCaptureLost)
    EVT_IDLE(GLAnimationCanvas::OnIdle)
    END_EVENT_TABLE()

    GLAnimationCanvas::GLAnimationCanvas(
    wxWindow *parent, 
    ECanvasType eType, 
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxString& name)
    : wxGLCanvas(parent, id, NULL, pos, size,
    style | wxFULL_REPAINT_ON_RESIZE, name)
    , m_iType(eType)
    , m_selectedIndex(0)
    , m_bLeftDown(false)
    , m_bRightDown(false)
{
    for (int i = 0; i < 26; i++)
    {
        m_KeyStates.push_back(*new bool(false));
    }

    m_pGLRC = new wxGLContext(this);
}

GLAnimationCanvas::~GLAnimationCanvas()
{
    BEATS_SAFE_DELETE(m_pGLRC);
}

void GLAnimationCanvas::ResetProjectionMode()
{
    if ( !IsShownOnScreen() )
        return;

    SetCurrent(*m_pGLRC);

    int w, h;
    GetClientSize(&w, &h);

    CRenderManager::GetInstance()->SetWindowSize(w, h);
}

void GLAnimationCanvas::InitGL()
{
    CRenderManager::GetInstance()->Initialize();
    m_Model = CRenderObjectManager::GetInstance()->CreateModel();
}

void GLAnimationCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // must always be here
    wxPaintDC dc(this);

    SetCurrent(*m_pGLRC);

    // Initialize OpenGL
    if (!m_gldata.initialized)
    {
        InitGL();

        ResetProjectionMode();
        m_gldata.initialized = true;
    }


    static DWORD last = timeGetTime();
    DWORD curr = timeGetTime();
    DWORD delta = curr - last;
    last = curr;
    CAnimationManager::GetInstance()->Update((float)delta/1000);
    CRenderManager::GetInstance()->Render();
    SwapBuffers();
}

void GLAnimationCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // Reset the OpenGL view aspect.
    // This is OK only because there is only one canvas that uses the context.
    // See the cube sample for that case that multiple canvases are made current with one context.
    ResetProjectionMode();
}

void GLAnimationCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void GLAnimationCanvas::OnMouse(wxMouseEvent& event)
{
    if (m_iType == TYPE_CURVE)
    {
        wxPoint wxpt = event.GetPosition();
        Spline::Point pt(wxpt.x, wxpt.y);
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            size_t index;
            if(m_spline->findKont(pt, index))
            {
                m_selectedIndex = index;
            }
            else
            {
                m_selectedIndex = -1;
            }
        }
        else if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            size_t index;
            if(m_spline->findKont(pt, index))
            {
                m_spline->removeKnot(index);
                m_selectedIndex = -1;
            }
            else
            {
                m_selectedIndex = m_spline->addKnots(pt);
            }
        }
        else if (event.Dragging())
        {
            if(m_selectedIndex >= 0)
            {
                m_spline->setKnot(m_selectedIndex, pt);    
            }
        }
    }
    else if (m_iType == TYPE_ANIMATION)
    {
        if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            ShowCursor(false);
            SetFocus();
            if (!HasCapture())
            {
                CaptureMouse();
            }
            m_bRightDown = true;
        }
        else if(event.ButtonUp(wxMOUSE_BTN_RIGHT))
        {
            ShowCursor(true);
            if (HasCapture())
            {
                ReleaseMouse(); 
            }
            ResetKeyStates();
            m_bRightDown = false;
        }
        else if(event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            ShowCursor(false);
            SetFocus();
            if (!HasCapture())
            {
                CaptureMouse();
            }
            m_bLeftDown = true;
        }
        else if(event.ButtonUp(wxMOUSE_BTN_LEFT))
        {
            ShowCursor(true);
            if (!HasCapture())
            {
                CaptureMouse();
            }
            ResetKeyStates();
            m_bLeftDown = false;
        }
        else if(event.Dragging())
        {
            wxPoint curPos = event.GetPosition();
            wxPoint pnt = ClientToScreen(curPos);
            SetCursorPos(pnt.x, pnt.y);
            CRenderManager* pRenderMgr = CRenderManager::GetInstance();
            if (m_bRightDown)
            {
                int nDeltaX = curPos.x - m_lastPosition.x;
                int nDeltaY = curPos.y - m_lastPosition.y;
                wxSize clientSize = GetClientSize();
                pRenderMgr->GetCamera()->Yaw((float)nDeltaX / clientSize.x);
                pRenderMgr->GetCamera()->Pitch((float)nDeltaY / clientSize.y);
            }
        }
        else if(event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
        {
            kmVec3 vec3Speed;
            kmVec3Fill(&vec3Speed, SHIFTWHEELSPEED, SHIFTWHEELSPEED, SHIFTWHEELSPEED);
            if (event.GetWheelRotation() > 0)
            {
                CRenderManager::GetInstance()->GetCamera()->Update(vec3Speed, (1 << CCamera::eCMT_STRAIGHT));
            }
            else if (event.GetWheelRotation() < 0)
            {
                kmVec3Scale(&vec3Speed, &vec3Speed, -1.0f);
                CRenderManager::GetInstance()->GetCamera()->Update(vec3Speed, (1 << CCamera::eCMT_STRAIGHT));
            }
        }
        m_lastPosition = event.GetPosition();
    }
}


void GLAnimationCanvas::MouseCaptureLost( wxMouseCaptureLostEvent& /*event*/ )
{
    ResetKeyStates();
    m_bRightDown = false;
    m_bLeftDown = false;
}


void GLAnimationCanvas::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static float fFrame = 0.0f;
    fFrame = fFrame + 0.016f;
    if ( fFrame > 0.1f )
    {
        fFrame = 0.0f;
        UpDateCamera();
        EditAnimationDialog* pDialog = (EditAnimationDialog*)GetParent();
        if (m_Model->GetAnimationController()->IsPlaying())
        {
            int iCur = m_Model->GetAnimationController()->GetCurrFrame();
            (pDialog->GetTimeBar())->SetCurrentCursor(iCur);
        }
    }
    Refresh(false);
}

void GLAnimationCanvas::SetType( ECanvasType iType )
{
    m_iType = iType;
}

void GLAnimationCanvas::OnKeyDown( wxKeyEvent& event )
{
    if (m_bRightDown)
    {
        int iKet = event.GetKeyCode();
        SetKeyState(iKet , true);
    }  
}


void GLAnimationCanvas::OnKeyUp( wxKeyEvent& event )
{
    if (m_bRightDown)
    {
        int iKet = event.GetKeyCode();
        SetKeyState(iKet , false);
    }
}


bool GLAnimationCanvas::GetKeyState( int iKey )
{
    bool bKey = false;
    switch (iKey)
    {
    case KEY_A:
        bKey = m_KeyStates[KEY_A];
        break;
    case KEY_S:
        bKey = m_KeyStates[KEY_S];
        break;
    case KEY_D:
        bKey = m_KeyStates[KEY_D];
        break;
    case KEY_W:
        bKey = m_KeyStates[KEY_W];
        break;
    case KEY_Q:
        bKey = m_KeyStates[KEY_Q];
        break;
    case KEY_Z:
        bKey = m_KeyStates[KEY_Z];
        break;
    case KEY_SHIFT:
        bKey = m_KeyStates[KEY_SHIFT];
        break;
    case KEY_CTRL:
        bKey = m_KeyStates[KEY_CTRL];
        break;
    default:
        break;
    }
    return bKey;
}

void GLAnimationCanvas::UpDateCamera()
{
    kmVec3 vec3Speed;
    kmVec3Fill(&vec3Speed, 1.0f, 1.0f, 1.0f);
    if ( GetKeyState(KEY_SHIFT) )
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 5.0F * 0.016F);
    }
    else
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 1.0F * 0.016F);
    }
    int type = CCamera::eCMT_NOMOVE;

    bool bPressA = GetKeyState(KEY_A);
    bool bPressD = GetKeyState(KEY_D);
    if ( bPressA || bPressD )
    {
        type |= (1 << CCamera::eCMT_TRANVERSE);
        if (bPressA)
        {
            vec3Speed.x *= -1;
        }
    }
    bool bPressW = GetKeyState(KEY_W);
    bool bPressS = GetKeyState(KEY_S);

    if ( bPressW || bPressS )
    {
        type |= (1 << CCamera::eCMT_STRAIGHT);
        if (bPressW)
        {
            vec3Speed.z *= -1;
        }
    }
    bool bPressUp = GetKeyState(KEY_Q);
    bool bPressDown = GetKeyState(KEY_Z);

    if ( bPressUp || bPressDown )
    {
        type |= (1 << CCamera::eCMT_UPDOWN);
        if (bPressDown)
        {
            vec3Speed.y *= -1;
        }
    }

    if (type != CCamera::eCMT_NOMOVE)
    {
        CRenderManager::GetInstance()->GetCamera()->Update(vec3Speed, type);
    }
}

void GLAnimationCanvas::SetKeyState( int iKeyCode, bool bKey )
{
    int iKey = -1;
    switch (iKeyCode)
    {
    case 'A':
        iKey = KEY_A;
        break;
    case 'S':
        iKey = KEY_S;
        break;
    case 'D':
        iKey = KEY_D;
        break;
    case 'W':
        iKey = KEY_W;
        break;
    case 'Q':
        iKey = KEY_Q;
        break;
    case 'Z':
        iKey = KEY_Z;
        break;
    case WXK_SHIFT:
        iKey = KEY_SHIFT;
        break;
    case WXK_CONTROL:
        iKey = KEY_CTRL;
        break;
    default:
        break;
    }
    if (iKey != -1)
    {
        m_KeyStates[iKey] = bKey;
    }

}

void GLAnimationCanvas::ResetKeyStates()
{
    for (auto i : m_KeyStates)
    {
        i = false;
    }
}

SharePtr<CModel>& GLAnimationCanvas::GetModel()
{
    return m_Model;
}
