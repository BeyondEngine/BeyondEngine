#ifndef BEYOND_ENGINE_SCENE_EVENTMANAGE_H__INCLUDE
#define BEYOND_ENGINE_SCENE_EVENTMANAGE_H__INCLUDE
enum EGameEventType
{
    eGET_UnlockUniverseCell,

    eGET_Count,
};
class CEventManager
{
    BEATS_DECLARE_SINGLETON(CEventManager);
public:
    void Update( float dtt );
    void RegisterHandler(EGameEventType eType, std::function<void()> handler);
    void PostEvent(EGameEventType eType);
    void HandleEvent(EGameEventType eType);

private:
    std::vector<EGameEventType> m_postedEventList;
    std::map<EGameEventType, std::function<void()>> m_handlerMap;
};

#endif