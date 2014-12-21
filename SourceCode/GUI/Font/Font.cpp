#include "stdafx.h"
#include "Font.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/FilePathTool.h"
FT_Library CFont::m_library = nullptr;
int CFont::m_nLibRefCount = 0;

CFont::CFont()
{
}

CFont::~CFont()
{
    if (IsInitialized())
    {
        Uninitialize();
    }
}

bool CFont::Load()
{
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a font which is already loaded!"));
    FT_Error err = 0;
    if(m_nLibRefCount++ == 0)
    {
        err = FT_Init_FreeType(&m_library);
        BEATS_ASSERT(!err, "call FT_Init_FreeType failed!");
    }
    BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(GetFilePath().c_str()), _T("Font file %s doesn't exits!"), GetFilePath().c_str());
    err = FT_New_Face(m_library, GetFilePath().c_str(), 0, &m_pFace);
    BEATS_ASSERT(!err, "Call FT_New_Face failed of file %s Error code: %d", GetFilePath().c_str(), err);
    BEATS_PRINT(_T("Load Font %s, glyph count: %d char map count %d\n"), GetFilePath().c_str(), (int32_t)m_pFace->num_glyphs, m_pFace->num_charmaps);

    err = FT_Select_Charmap(m_pFace, FT_ENCODING_UNICODE);
    BEATS_ASSERT(!err, "FT_Select_Charmap failed! Error code:%d", err);

    super::Load();
    return !err;
}

bool CFont::Unload()
{
    BEATS_ASSERT(IsLoaded(), _T("Can't Unload a font which haven't been loaded!"));

    FT_Error err = FT_Done_Face(m_pFace);
    BEATS_ASSERT(!err);

    if(--m_nLibRefCount == 0)
    {
        err = FT_Done_FreeType(m_library);
        BEATS_ASSERT(!err);
    }
    super::Unload();
    return !err;
}

FT_Face CFont::GetFontFace() const
{
    return m_pFace;
}

FT_Library CFont::GetLibrary()
{
    return m_library;
}
