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
}

CGradientColorBoard::~CGradientColorBoard()
{

}

void CGradientColorBoard::OnPaint(wxPaintEvent& /*event*/)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC dc(this);
    DrawBackGround(&dc);
    dc.DrawBitmap(wxBitmap(m_mergeImage), 0, 0);
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
            pDc->SetBrush((i + j) % 2 == 0 ? *wxWHITE : *wxLIGHT_GREY);
            pDc->DrawRectangle(i * BACKGROUNDSQUARESIZE, j * BACKGROUNDSQUARESIZE, BACKGROUNDSQUARESIZE, BACKGROUNDSQUARESIZE);
        }
    }
}

const wxImage& CGradientColorBoard::GetImage() const
{
    return m_mergeImage;
}

void CGradientColorBoard::SetBmp(wxBitmap& colorBmp, wxBitmap& alphaBmp)
{
    m_foreGroundBmp = colorBmp;
    m_alphaBmp = alphaBmp;

    m_mergeImage = m_foreGroundBmp.ConvertToImage();
    wxImage alphaImage = m_alphaBmp.ConvertToImage();
    if (!m_mergeImage.HasAlpha())
    {
        m_mergeImage.InitAlpha();
    }

    int nColumnCount = GetSize().x;
    int nRowCount = GetSize().y;
    for (int i = 0; i < nColumnCount; i++)
    {
        for (int j = 0; j < nRowCount; j++)
        {
            m_mergeImage.SetAlpha(i, j, alphaImage.GetRed(i, j));
        }
    }
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
        image = m_alphaBmp.ConvertToImage();
    }
    nPositionX = nPositionX >= image.GetWidth() ? nPositionX - 1 : nPositionX;
    BEATS_ASSERT(eType != eCT_Invalid);
    wxColor color(image.GetRed(nPositionX, nPositionY)
                , image.GetGreen(nPositionX, nPositionY)
                , image.GetBlue(nPositionX, nPositionY));
    return color;
}

wxColor CGradientColorBoard::GetColorWithAlpha(wxPoint point)
{
    static int nPositionY = 1;
    if (m_mergeImage.GetSize().GetWidth() <= point.x)
    {
        point.x = m_mergeImage.GetSize().GetWidth() - 1;
    }
    BEATS_ASSERT(m_mergeImage.HasAlpha());
    wxColor color(m_mergeImage.GetRed(point.x, nPositionY)
        , m_mergeImage.GetGreen(point.x, nPositionY)
        , m_mergeImage.GetBlue(point.x, nPositionY)
        , m_mergeImage.GetAlpha(point.x, nPositionY));
    return color;
}
