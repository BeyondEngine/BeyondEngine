#include "stdafx.h"
#include "ScriptManager.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "tolua++.h"

CScriptManager* CScriptManager::m_pInstance = nullptr;
static int Lua_WriteData(lua_State* L)
{
    BEATS_ASSERT(lua_isnumber(L, -1) && lua_isstring(L, -2) && lua_isnumber(L, -3));
    size_t uDataLength = lua_tointeger(L, -1);
    const char* pszData = lua_tolstring(L, -2, &uDataLength);
    void* pAddress = (void*)((uint32_t)tolua_tonumber(L, -3, 0));
    BEATS_ASSERT(pAddress != nullptr);
    memcpy(pAddress, pszData, uDataLength);
    return 1;
}

static int Lua_ReadData(lua_State* L)
{
    BEATS_ASSERT(lua_isnumber(L, -1) && lua_isnumber(L, -2));
    size_t uDataLength = lua_tointeger(L, -1);
    void* pAddress = (void*)(uint32_t)(tolua_tonumber(L, -2, 0));
    BEATS_ASSERT(pAddress != nullptr);
    lua_pushlstring(L, (const char*)pAddress, uDataLength);
    return 1;
}

static int Lua_DebugPrintIndex(lua_State* m_pLuaState, int index)
{
    char szBuffer[256] = { 0 };
    int nType = lua_type(m_pLuaState, index);
    switch (nType)
    {
    case LUA_TNIL:
        _stprintf(szBuffer, "nil");
        break;
    case LUA_TBOOLEAN:
        _stprintf(szBuffer, lua_toboolean(m_pLuaState, index) ? "true" : "false");
        break;
    case LUA_TNUMBER:
        _stprintf(szBuffer, _T("%g"), lua_tonumber(m_pLuaState, index));
        break;
    case LUA_TSTRING:
        _stprintf(szBuffer, _T("%s"), lua_tostring(m_pLuaState, index));
        break;
    case LUA_TUSERDATA:
        _stprintf(szBuffer, _T("%p"), lua_touserdata(m_pLuaState, index));
        break;
    default:
        _stprintf(szBuffer, _T("--"));
        break;
    }
    BEATS_PRINT(_T("Type:%d Value: %s\n"), nType, szBuffer);
    return 1;
}

static int Lua_DebugPrint(lua_State* L)
{
    Lua_DebugPrintIndex(L, -1);
    return 1;
}

static int Lua_DebugPrintStack(lua_State*)
{
    CScriptManager::GetInstance()->PrintStack();
    return 1;
}

static int Lua_GetUserDataValue(lua_State* L)
{
    BEATS_ASSERT(lua_isuserdata(L, -1));
    void* pUserData = tolua_touserdata(L, -1, nullptr);
    tolua_pushnumber(L, (LUA_NUMBER)(size_t)pUserData);
    return 1;
}

static int Lua_RegisterEventCallBack(lua_State* L)
{
    BEATS_ASSERT(L != nullptr);
    BEATS_ASSERT(lua_isfunction(L, -1) && lua_isnumber(L, -2) && lua_isstring(L, -3));
    int32_t nFunctionRef = luaL_ref(L, LUA_REGISTRYINDEX);
    std::string pUserData = tolua_tostring(L, -2, nullptr);
    BEATS_ASSERT(!pUserData.empty(), "Register event call back failed with object null!");
    uint32_t uEventID = (uint32_t)tolua_tonumber(L, -1, 0);
    CScriptManager::GetInstance()->RegisterScriptFunc(pUserData, uEventID, nFunctionRef);
    return 1;
}

static int Lua_UnRegisterEventCallBack(lua_State* L)
{
    BEATS_ASSERT(L != nullptr);
    BEATS_ASSERT(lua_isnumber(L, -1) && lua_isstring(L, -2));
    std::string pUserData = tolua_tostring(L, -2, nullptr);
    BEATS_ASSERT(!pUserData.empty());
    uint32_t uEventId = (uint32_t)tolua_tonumber(L, -1, 0);
    CScriptManager::GetInstance()->UnregisterScriptFunc(pUserData, uEventId);
    return 1;
}

CScriptManager::CScriptManager()
: m_pLuaState(nullptr)
{

}

CScriptManager::~CScriptManager()
{
    Uninitialize();
}

void CScriptManager::Initialize()
{
    if (!m_pLuaState)
    {
        m_pLuaState = lua_open();
        BEATS_ASSERT(m_pLuaState != nullptr);
        luaL_openlibs(m_pLuaState);
        TString strScriptPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Script);
        strScriptPath.append("/publicdef.lua");
        CSerializer publicDefLua(strScriptPath.c_str());
        BEATS_ASSERT(publicDefLua.GetWritePos() > 0);
        int nRet = luaL_loadbuffer(m_pLuaState, (const char*)publicDefLua.GetBuffer(), publicDefLua.GetWritePos(), "publicdef.lua");
        BEYONDENGINE_UNUSED_PARAM(nRet);
        BEATS_ASSERT(nRet == 0, _T("Load file %s failed in CScriptManager::Initialize!\n"), strScriptPath.c_str());
        lua_pcall(m_pLuaState, 0, LUA_MULTRET, 0);

        lua_register(m_pLuaState, "Lua_WriteData", Lua_WriteData);
        lua_register(m_pLuaState, "Lua_ReadData", Lua_ReadData);
        lua_register(m_pLuaState, "Lua_GetUserDataValue", Lua_GetUserDataValue);
        lua_register(m_pLuaState, "Lua_RegEventCallback", Lua_RegisterEventCallBack);
        lua_register(m_pLuaState, "Lua_UnRegEventCallback", Lua_UnRegisterEventCallBack);
        lua_register(m_pLuaState, "print", Lua_DebugPrint);
        lua_register(m_pLuaState, "PrintStack", Lua_DebugPrintStack);
        BEATS_ASSERT(lua_gettop(m_pLuaState) == 0);
    }
}

void CScriptManager::Uninitialize()
{
    if (nullptr != m_pLuaState)
    {
        lua_close(m_pLuaState);
        m_pLuaState = nullptr;
    }
}

int CScriptManager::ExecuteStack(int nArgc, int nReturnc)
{
    int ret = 0;
    BEATS_ASSERT(std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), "script can be execute only in main thread!");
    int error = lua_pcall(m_pLuaState, nArgc, nReturnc, 0);                  /* L: ... [G] ret */
    if (error != 0)
    {
        BEATS_ASSERT(false, "[LUA ERROR] %s", lua_tostring(m_pLuaState, -1));        /* L: ... error */
        lua_pop(m_pLuaState, 1); // remove error message from stack
    }
    return ret;
}

bool CScriptManager::IsFuncRegister(std::string pUserData, uint32_t uEventId)
{
    bool reg = false;
    auto iter = m_scriptFunctionMap.find(pUserData);
    if (iter != m_scriptFunctionMap.end())
    {
        auto subIter = iter->second.find(uEventId);
        if (subIter != iter->second.end())
            reg = true;
    }
    return reg;
}

bool CScriptManager::LoadRegisterFunc(std::string pUserData, uint32_t uEventId)
{
    bool bRet = false;
    auto iter = m_scriptFunctionMap.find(pUserData);
    if (iter != m_scriptFunctionMap.end())
    {
        auto subIter = iter->second.find(uEventId);
        if (subIter != iter->second.end())
        {
            lua_getref(m_pLuaState, subIter->second);
            bRet = true;
        }
    }
    BEATS_ASSERT(bRet, "LoadRegisterFunc : %s event %d is not found", pUserData.c_str(), uEventId);
    return bRet;
}

int CScriptManager::ExecuteUIScriptFunc(std::string pUserData, uint32_t uEventId)
{
    BEATS_ASSERT(std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), "script can be execute only in main thread!");
    auto iter = m_scriptFunctionMap.find(pUserData);
    if (iter != m_scriptFunctionMap.end())
    {
        auto subIter = iter->second.find(uEventId);
        if (subIter != iter->second.end())
        {
            lua_getref(m_pLuaState, subIter->second);
            BEATS_ASSERT(lua_isfunction(m_pLuaState, -1));
            ExecuteStack(0, 0);
        }
    }
    return 1;
}

void CScriptManager::PrintStack()
{
    if (m_pLuaState)
    {
        int nStackTop = lua_gettop(m_pLuaState);
        if (nStackTop > 0)
        {
            BEATS_PRINT(_T("Lua stack with %d elements(From top to bottom):\n"), nStackTop);
            char szBuffer[256] = { 0 };
            for (int i = 0; i < nStackTop; ++i)
            {
                int nIndex = -1 - i;
                int nType = lua_type(m_pLuaState, nIndex);
                switch (nType)
                {
                case LUA_TNIL:
                    _stprintf(szBuffer, "nil");
                    break;
                case LUA_TBOOLEAN:
                    _stprintf(szBuffer, lua_toboolean(m_pLuaState, nIndex) ? "true" : "false");
                    break;
                case LUA_TNUMBER:
                    _stprintf(szBuffer, _T("%g"), lua_tonumber(m_pLuaState, nIndex));
                    break;
                case LUA_TSTRING:
                    _stprintf(szBuffer, _T("%s"), lua_tostring(m_pLuaState, nIndex));
                    break;
                case LUA_TUSERDATA:
                    _stprintf(szBuffer, _T("%p"), lua_touserdata(m_pLuaState, nIndex));
                    break;
                default:
                    _stprintf(szBuffer, _T("--"));
                    break;
                }
                const char* pszTypeName = lua_typename(m_pLuaState, nType);
                BEYONDENGINE_UNUSED_PARAM(pszTypeName);
                BEATS_PRINT(_T("Index: %d Type:%d %s Value: %s\n"), i, nType, pszTypeName, szBuffer);
            }
        }
        else
        {
            BEATS_PRINT("The stack is empty!\n");
        }
    }
}

lua_State* CScriptManager::GetLuaState() const
{
    return m_pLuaState;
}

void CScriptManager::RegisterScriptFunc(std::string pUserData, uint32_t uEventId, int32_t funcid)
{
    auto iter = m_scriptFunctionMap.find(pUserData);
    if (iter != m_scriptFunctionMap.end())
    {
        iter->second[uEventId] = funcid;
    }
    else
    {
        std::map<uint32_t, int32_t> eventmap;
        eventmap[uEventId] = funcid;
        m_scriptFunctionMap[pUserData] = eventmap;
    }
    
}

void CScriptManager::UnregisterScriptFunc(std::string pUserData, uint32_t uEventId)
{
    auto iter = m_scriptFunctionMap.find(pUserData);
    if (iter != m_scriptFunctionMap.end())
    {
        if (uEventId > 0)
        {
            auto subIter = iter->second.find(uEventId);
            if (subIter != iter->second.end())
            {
                lua_unref(m_pLuaState, subIter->second);
                iter->second.erase(subIter);
            }
        }
        else
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                lua_unref(m_pLuaState, subIter->second);
            }
            m_scriptFunctionMap.erase(iter);
        }
    }
}

void CScriptManager::UnRegisterAllFunc()
{
    auto iter = m_scriptFunctionMap.begin();
    for (; iter != m_scriptFunctionMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            lua_unref(m_pLuaState, subIter->second);
        }
        iter->second.clear();
    }
    m_scriptFunctionMap.clear();
}

const std::map<std::string, std::map<uint32_t, int32_t>>& CScriptManager::GetScriptFuncMap() const
{
    return m_scriptFunctionMap;
}

int CScriptManager::ExecuteScriptFile(const std::string& filename)
{
    int nRet = 0;
    if (!filename.empty())
    {
        BEATS_ASSERT(std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), "script can be execute only in main thread!");
        TString strScriptPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Script);
        strScriptPath.append("/");
        TString strLowerName = CStringHelper::GetInstance()->ToLower(filename);
        strScriptPath.append(strLowerName);
        bool bFileExists = CFilePathTool::GetInstance()->Exists(strScriptPath.c_str());
        BEATS_ASSERT(bFileExists, "Can't file lua file %s", strScriptPath.c_str());
        if (bFileExists)
        {
            BEYONDENGINE_CHECK_HEAP;
            CSerializer luaFileData(strScriptPath.c_str());
            BEATS_ASSERT(luaFileData.GetWritePos() > 0);
            nRet = luaL_loadbuffer(m_pLuaState, (const char*)luaFileData.GetBuffer(), luaFileData.GetWritePos(), strLowerName.c_str());
            BEYONDENGINE_UNUSED_PARAM(nRet);
            BEATS_ASSERT(nRet == 0, _T("Load file %s failed in CScriptManager::executeScriptFile!\n"), strScriptPath.c_str());
            ExecuteStack(0, 0);
            lua_settop(m_pLuaState, 0);
            BEYONDENGINE_CHECK_HEAP;
        }
    }
    return nRet;
}