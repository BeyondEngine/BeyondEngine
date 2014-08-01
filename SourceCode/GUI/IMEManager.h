#ifndef BEYOND_ENGINE_GUI_IMEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_GUI_IMEMANAGER_H__INCLUDE

class CIMEManager
{
    BEATS_DECLARE_SINGLETON(CIMEManager);
public:

    void SetKeybordState( bool bOpen );
protected:
private:
};

#endif