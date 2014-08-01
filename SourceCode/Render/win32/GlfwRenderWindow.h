#ifndef BEYOND_ENGINE_RENDER_WIN32_GLFWRENDERWINDOW_H__INCLUDE
#define BEYOND_ENGINE_RENDER_WIN32_GLFWRENDERWINDOW_H__INCLUDE

#include "Render/RenderWindow.h"

struct GLFWwindow;
class CGlfwRenderWindow : public CRenderWindow
{
    typedef CRenderWindow super;
public:
    CGlfwRenderWindow(int nWidth, int nHeight, bool bAutoCreateViewport = false);
    virtual ~CGlfwRenderWindow();

    virtual void SetToCurrent() override;
    virtual void Render() override;

    GLFWwindow *GetMainWindow() const;

private:
    bool InitGlew();
    bool BindingFBO();
    void UpdateCamera();
    bool IsLeftMouseDown() { return m_bLeftMouseDown; }
    void GetMousePos(size_t& x, size_t& y) { x = m_uCurMousePosX, y = m_uCurMousePosY; }

    static void onGLFWError(int errorID, const char* errorDesc);
    static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
    static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
    static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
    static void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
    static void onGLFWframebuffersize(GLFWwindow* window, int w, int h);

private:
    GLFWwindow* m_pMainWindow;
    bool m_bGlewInited;
    bool m_bLeftMouseDown;
    size_t m_uLastMousePosX;
    size_t m_uLastMousePosY;
    size_t m_uCurMousePosX;
    size_t m_uCurMousePosY;
    size_t m_uPressedMousePosX;
    size_t m_uPressedMousePosY;
    float m_fPressStartYaw;
    float m_fPressStartPitch;
    size_t m_uLastTapTime;
    unsigned int m_uTapCount;

    static const size_t TAP_INTERVAL = 250;
    static const size_t TAP_DEVIATION = 5;
    static std::map<GLFWwindow *, CGlfwRenderWindow *> m_sInstanceMap;
};

#endif