#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCURSOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCURSOR_H__INCLUDE

enum ECursorType
{
    eCT_Invalid = -1,

    eCT_Alpha,
    eCT_Color,
};

class CGradientCursor
{
public:
    CGradientCursor(float fPos, wxColor color, ECursorType eType);
    ~CGradientCursor();

    void SetPos(float fPos);
    float GetPosPercent() const;
    void SetColor(wxColor color);
    wxColor& GetColor();
    void Select(bool bSelect);
    bool IsSelected();
    ECursorType GetType() const;

    bool operator!=(const CGradientCursor& obj) const;
    bool operator==(const CGradientCursor& obj) const;
    bool operator<(const CGradientCursor& obj) const;

private:
    bool m_bSelected;
    float m_fPos;
    ECursorType m_eType;
    wxColor m_color;
};
#endif