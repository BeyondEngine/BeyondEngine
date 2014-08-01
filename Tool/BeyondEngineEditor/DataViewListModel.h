#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWLISTMODEL_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWLISTMODEL_H__INCLUDE

#include <wx/dynarray.h>
#include <wx/dataview.h>

enum
{
    Col_EditableText,
    Col_Visible,
    Col_Lock,
    Col_Max
};

WX_DEFINE_ARRAY_CHAR(bool, wxArrayBool);
class CDataViewListModel: public wxDataViewListStore
{
public:
    CDataViewListModel();
    virtual ~CDataViewListModel();
};


#endif