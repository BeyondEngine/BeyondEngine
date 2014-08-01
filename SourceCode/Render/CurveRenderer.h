#ifndef BEYOND_ENGINE_RENDER_CURVERENDERER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_CURVERENDERER_H__INCLUDE

#include "CommonTypes.h"

struct SSpline;
class CShaderProgram;
class CCurveRenderer
{
public:
    CCurveRenderer(SharePtr<SSpline> spline);
    ~CCurveRenderer();

    virtual void PreRender();
    virtual void DoRender();
    virtual void PostRender();

    void SetProgram(CShaderProgram* program);

private:
    void RenderGrid();
    void RenderSpline();
    void RenderPoints();

private:
    CShaderProgram* m_pProgram;
    SharePtr<SSpline> m_spline;
    GLuint m_VBOGrid;
    GLuint m_VBOSpline;
    GLuint m_VBOPoints;

    static const int SPLINE_SEGMENTS = 32;
    static const int GRID_X_MAX = 500;
    static const int GRID_Y_MAX = 500;
    static const int GRID_WIDTH = 50;
    static const int GRID_HEIGHT = 50;
    static const int GRID_VERTEX_NUM = (GRID_X_MAX/GRID_WIDTH+1)*2 + (GRID_Y_MAX/GRID_HEIGHT+1)*2;
    static const CColor GRID_COLOR;
    static const CColor SPLINE_COLOR;
    static const CColor POINT_COLOR;
};

#endif