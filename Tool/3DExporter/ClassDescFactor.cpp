#include "stdafx.h"
#include "ClassDescFactor.h"
#include "3DSceneExport.h"

ClassDescFactor::ClassDescFactor()
{
}

ClassDescFactor::~ClassDescFactor()
{
}

int ClassDescFactor::IsPublic() 
{
    return TRUE;
}

void* ClassDescFactor::Create(BOOL/* loading*/)
{
    return new C3DSceneExport(); 
} 

const MCHAR* ClassDescFactor::ClassName() 
{ 
#ifdef _DEBUG
    return _T("Beyond Engine Exporterd"); 
#else
    return _T("Beyond Engine Exporterr");
#endif
}

SClass_ID ClassDescFactor::SuperClassID()
{
    return SCENE_EXPORT_CLASS_ID; 
}

Class_ID ClassDescFactor::ClassID() 
{
    return Class_ID(0x726478d, 0xc706974);
}

const MCHAR* ClassDescFactor::Category()
{
#ifdef    _DEBUG
    return _T("Beyond Engine Exporterd Category Combined");
#else
    return _T("Beyond Engine Exporterr Category Combined");
#endif
}

MCHAR* ClassDescFactor::GetRsrcString(INT_PTR /*id*/)
{
    return _T("");
}