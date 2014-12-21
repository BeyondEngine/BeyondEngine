#ifndef BEYOND_ENGINE_SCRIPTMANAGER_H__INCLUDE
#define BEYOND_ENGINE_SCRIPTMANAGER_H__INCLUDE

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CScriptManager
{
    BEATS_DECLARE_SINGLETON(CScriptManager);
public:
    void Initialize();
    void Uninitialize();
    int ExecuteStack(int nArgc, int nReturnc = 1);
    int ExecuteUIScriptFunc(std::string, uint32_t uEventId);
    bool IsFuncRegister(std::string, uint32_t uEventId);
    bool LoadRegisterFunc(std::string, uint32_t uEventId);
    void PrintStack();
    lua_State* GetLuaState() const;
    void RegisterScriptFunc(std::string pUserData, uint32_t uEventId, int32_t funcid);
    void UnregisterScriptFunc(std::string pUserData, uint32_t uEventId);
    void UnRegisterAllFunc();
    const std::map<std::string, std::map<uint32_t, int32_t>>& GetScriptFuncMap() const;
    int ExecuteScriptFile(const std::string& filename);
private:
    lua_State* m_pLuaState = nullptr;
    std::map<std::string, std::map<uint32_t, int32_t>> m_scriptFunctionMap; // In lua, the function map is <functionRef, function>
};

#endif