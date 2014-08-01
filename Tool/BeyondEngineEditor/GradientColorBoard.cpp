#include "stdafx.h"
#include "GradientColorBoard.h"
#include "wx/gdicmn.h"
#include "wx/dcgraph.h"
#include "wx/dcbuffer.h"

#define BACKGROUNDSQUARESIZE 10

BEGIN_EVENT_TABLE(CGradientColorBoard, wxPanel)
    EVT_PAINT(CGradientColorBoard::OnPaint)
END_EVENT_TABLE()

CGradientColorBoard::CGradientColorBoard(wxWindow *parent, wxWindowID id , const wxPoint& pos , const wxSize& size , long style , const wxString& name)
    : wxPanel(parent, id, pos, size, style, name)
{
    InitCtrl();
}

CGradientColorBoard::~CGradientColorBoard()
{

}

void CGradientColorBoard::InitCtrl()
{
    
}

void CGradientColorBoard::OnPaint(wxPaintEvent& /*event*/)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC dc(this);
    DrawBackGround(&dc);
    
    dc.DrawBitmap(m_drawBmp, 0, 0);
}

void CGradientColorBoard::DrawBackGround(wxDC* pDc)
{
    wxRect rect = GetRect();
    int nColumnCount = rect.width / BACKGROUNDSQUARESIZE + 1;
    int nRowCount = rect.height / BACKGROUNDSQUARESIZE + 1;
    pDc->SetPen(*wxWHITE_PEN);
    for (int i = 0; i < nColumnCount; i++)
    {
        for (int j = 0; j < nRowCount; j++)
        {
            if ((i + j) % 2 == 0)
            {
                pDc->SetBrush(*wxWHITE);
            }
            else
            {
                pDc->SetBrush(*wxLIGHT_GREY);
            }
            pDc->DrawRectangle(i * BACKGROUNDSQUARESIZE, j * BACKGROUNDSQUARESIZE, BACKGROUNDSQUARESIZE, BACKGROUNDSQUARESIZE);
        }
    }
}

wxBitmap& CGradientColorBoard::GetForeGroundBmp()
{
    return m_foreGroundBmp;
}

void CGradientColorBoard::SetBmp(wxBitmap& colorBmp, wxBitmap& maskBmp)
{
    m_foreGroundBmp = colorBmp;
    m_maskBmp = maskBmp;

    wxImage colorImage = m_foreGroundBmp.ConvertToImage();
    wxImage maskImage = m_maskBmp.ConvertToImage();
    if (!colorImage.HasAlpha())
    {
        colorImage.InitAlpha();
    }

    int nColumnCount = GetSize().x;
    int nRowCount = GetSize().y;
    for (int i = 0; i < nColumnCount; i++)
    {
        for (int j = 0; j < nRowCount; j++)
        {
            colorImage.SetAlpha(i, j, maskImage.GetRed(i, j));
        }
    }
    m_drawBmp = wxBitmap(colorImage);
}

wxColor CGradientColorBoard::GetColor(int nPositionX, ECursorType eType)
{
    static int nPositionY = 1;
    wxImage image;
    if (eType == eCT_Color)
    {
        image = m_foreGroundBmp.ConvertToImage();
    }
    else if (eType == eCT_Alpha)
    {
        image = m_maskBmp.ConvertToImage();
    }
    BEATS_ASSERT(eType != eCT_Invalid);
    wxColor color(image.GetRed(nPositionX, nPositionY)
                , image.GetGreen(nPositionX, nPositionY)
                , image.GetBlue(nPositionX, nPositionY));
    return color;
}

wxColor CGradientColorBoard::GetColorWithAlpha(wxPoint point)
{
    static int nPositionY = 1;
    wxImage image = m_drawBmp.ConvertToImage();
    if (image.GetSize().GetWidth() == point.x)
    {
        point.x -= 1;
    }
    if (!image.HasAlpha())
    {
        image.InitAlpha();
    }
    wxColor color(image.GetRed(point.x, nPositionY)
        , image.GetGreen(point.x, nPositionY)
        , image.GetBlue(point.x, nPositionY)
        , image.GetAlpha(point.x, nPositionY));
    return color;
}
