#ifndef BEYONDENGINEEDITOR_GLCANVAS_H__INCLUDE
#define BEYONDENGINEEDITOR_GLCANVAS_H__INCLUDE

#include "wx/glcanvas.h"
#include "Render/Model.h"

struct Spline;

struct sGLData
{
    sGLData():initialized(false){}
    bool initialized;           // have OpenGL been initialized?
    float beginx, beginy;       // position of mouse
    float quat[4];              // orientation of object
    float zoom;                 // field of view in degrees
};

class GLAnimationCanvas : public wxGLCanvas
{
    
public:
    enum ECanvasType
    {
        TYPE_ANIMATION,
        TYPE_CURVE
    };

    enum
    {
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_W,
        KEY_Q,
        KEY_Z,
        KEY_SHIFT,
        KEY_CTRL
    };

    GLAnimationCanvas(wxWindow *parent, ECanvasType eType = TYPE_ANIMATION, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("Animation"));

    virtual ~GLAnimationCanvas();
    SharePtr<CModel>& GetModel();
    void LoadDXF(const wxString& filename);
    void SetType(ECanvasType iType);
protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse(wxMouseEvent& event);
    void MouseCaptureLost(wxMouseCaptureLostEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    bool GetKeyState(int iKey);
    void SetKeyState(int iKeyCode, bool bKey);
    void UpDateCamera();
private:
    void InitGL();
    void ResetKeyStates();
    void ResetProjectionMode();

    
    ECanvasType     m_iType;
    int             m_selectedIndex;
    bool            m_bLeftDown;
    bool            m_bRightDown;
    wxPoint         m_lastPosition;
    std::vector<bool> m_KeyStates;
    SharePtr<Spline> m_spline;
    SharePtr<CModel> m_Model;
    wxGLContext*    m_pGLRC;
    sGLData         m_gldata;

    wxDECLARE_NO_COPY_CLASS(GLAnimationCanvas);
    DECLARE_EVENT_TABLE()
};

#endif