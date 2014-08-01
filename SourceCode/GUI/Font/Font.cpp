#include "stdafx.h"
#include "Font.h"
#include "Utility/BeatsUtility/StringHelper.h"

FT_Library CFont::m_library = nullptr;
int CFont::m_nLibRefCount = 0;

CFont::CFont()
    : m_pFace(nullptr)
{
}

CFont::~CFont()
{

}

bool CFont::Load()
{
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a font which is already loaded!"));
    m_pData = new CSerializer(GetFilePath().c_str());
    if(m_nLibRefCount++ == 0)
    {
        FT_Error err = FT_Init_FreeType(&m_library);
        err;
        BEATS_ASSERT(!err);
    }
    BEATS_ASSERT(m_pData->GetWritePos() != 0, _T("load font file %s failed!"), GetFilePath().c_str());
    // TODO: Figure out why not use FT_New_Face
    FT_Error err = FT_New_Memory_Face(m_library, m_pData->GetBuffer(), (FT_Long)m_pData->GetWritePos(), 0 ,&m_pFace);
    BEATS_ASSERT(!err);
    BEATS_PRINT(_T("Load Font %s, glyph count: %d char map count %d\n"), GetFilePath().c_str(), m_pFace->num_glyphs, m_pFace->num_charmaps);

    err = FT_Select_Charmap(m_pFace, FT_ENCODING_UNICODE);
    BEATS_ASSERT(!err);

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
        FT_Error err = FT_Done_FreeType(m_library);
        err;
        BEATS_ASSERT(!err);
    }

    BEATS_SAFE_DELETE(m_pData);

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