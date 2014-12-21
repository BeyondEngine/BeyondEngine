#ifndef BEYOND_ENGINE_SCRIPT_TOLUA_LUABASICCONVERSIONS_H__INCLUDE
#define BEYOND_ENGINE_SCRIPT_TOLUA_LUABASICCONVERSIONS_H__INCLUDE

extern "C" 
{
    #include "lua.h"
    #include "tolua++.h"
}
#include <unordered_map>

extern std::unordered_map<std::string, std::string>  g_luaType;
extern std::unordered_map<std::string, std::string>  g_typeCast;

void luaval_to_native_err(lua_State* L,const char* msg,tolua_Error* err, const char* funcName = "");

extern bool luaval_is_usertype(lua_State* L,int lo,const char* type, int def);
extern bool luaval_to_color(lua_State* L, int lo, CColor* outValue, const char* funcName = "");

template <class T>
inline bool luaval_to_object(lua_State* L, int lo, const char* type, T** ret, const char* funcName = "")
{
    bool bRet = false;
    bool bValidIndex = nullptr != L && lua_gettop(L) >= lo;
    BEATS_ASSERT(bValidIndex, _T("Lua index may not valid %d"), lo);
    if (bValidIndex)
    {
        bool bIsUserType = luaval_is_usertype(L, lo, type, 0);
        BEATS_ASSERT(bIsUserType, _T("%s is not a user type!"), type);
        if (bIsUserType)
        {
            void* pValue = tolua_tousertype(L, lo, 0);
            BEYONDENGINE_UNUSED_PARAM(funcName);
            BEATS_ASSERT(pValue != nullptr, "Warning: %s argument %d is invalid native object(nullptr)", funcName, lo);
            if (pValue != nullptr)
            {
                *ret = static_cast<T*>(pValue);
                bRet = true;
            }
        }
    }
    return true;
}

template <class T>
inline const char* getLuaTypeName(T* ret, const char* type)
{
    const char* pszRet = nullptr;
    if (nullptr != ret)
    {
        std::string hashName = typeid(*ret).name();
        auto iter = g_luaType.find(hashName);
        if (g_luaType.end() != iter)
        {
            pszRet = iter->second.c_str();
        }
        else
        {
            pszRet = type;
        }
    }

    return pszRet;
}

template <class T>
inline void object_to_luaval(lua_State* L, const char* type, T* ret)
{
    if (nullptr != ret)
    {
        tolua_pushusertype(L, (void*)ret, getLuaTypeName(ret, type));
    }
    else
    {
        lua_pushnil(L);
    }
}

// Get value from lua
template<class T>
inline bool GetValueFromLua(lua_State* , int , T* , const char* )
{
    BEATS_ASSERT(false, _T("Not implemented yet!"));
    return false;
}

#define DECLARE_GET_VALUE_FROM_LUA_TYPE(type, checkfunction, convertfunction)\
template <>\
inline bool GetValueFromLua(lua_State* L, int lo, type* outValue, const char* funcName)\
{\
    bool bRet = nullptr != L && nullptr != outValue; \
    if (bRet)\
    {\
        tolua_Error tolua_err; \
        if (!checkfunction(L, lo, 0, &tolua_err))\
        {\
            luaval_to_native_err(L, "#ferror:", &tolua_err, funcName); \
            bRet = false; \
        }\
        else\
        {\
            *outValue = (type)convertfunction(L, lo, 0); \
        }\
    }\
    return bRet; \
}

DECLARE_GET_VALUE_FROM_LUA_TYPE(short, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(unsigned short, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(char, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(unsigned char, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(float, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(double, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(long, tolua_isnumber, tolua_tonumber)
DECLARE_GET_VALUE_FROM_LUA_TYPE(unsigned long, tolua_isnumber, tolua_tonumber)

#pragma warning(disable:4800) // For bool, tolua_toboolean returns an int, which will cause an warning.
DECLARE_GET_VALUE_FROM_LUA_TYPE(bool, tolua_isboolean, tolua_toboolean)
#pragma warning(default:4800)
DECLARE_GET_VALUE_FROM_LUA_TYPE(std::string, tolua_iscppstring, tolua_tocppstring)
template <>
inline bool GetValueFromLua(lua_State* L, int lo, CColor* outValue, const char* funcName)
{
    uint32_t nValue = 0;
    bool bRet = GetValueFromLua(L, lo, &nValue, funcName);
    *outValue = nValue;
    return bRet;
}

template <>
inline bool GetValueFromLua(lua_State* L, int lo, CVec2* outValue, const char* funcName)
{
    BEYONDENGINE_UNUSED_PARAM(funcName);
    bool bRet = false;
    if (nullptr != L && nullptr != outValue)
    {
        tolua_Error tolua_err;
        bRet = tolua_istable(L, lo, 0, &tolua_err) != 0;
        if (bRet)
        {
            lua_pushstring(L, "x");
            lua_gettable(L, lo);
            outValue->X() = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pushstring(L, "y");
            lua_gettable(L, lo);
            outValue->Y() = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
#ifdef _DEBUG
        else
        {
            luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
        }
#endif
    }
    return bRet;
}

template <>
inline bool GetValueFromLua(lua_State* L, int lo, CVec3* outValue, const char* funcName)
{
    bool bRet = GetValueFromLua(L, lo, (CVec2*)outValue, funcName);
    if (bRet)
    {
        lua_pushstring(L, "z");
        lua_gettable(L, lo);
        outValue->Z() = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return bRet;
}

template <>
inline bool GetValueFromLua(lua_State* L, int lo, CVec4* outValue, const char* funcName)
{
    bool bRet = GetValueFromLua(L, lo, (CVec3*)outValue, funcName);
    if (bRet)
    {
        lua_pushstring(L, "w");
        lua_gettable(L, lo);
        outValue->W() = lua_isnil(L, -1) ? 0 : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    return bRet;
}

template <>
inline bool GetValueFromLua(lua_State* L, int lo, CMat4* outValue, const char* funcName)
{
    BEYONDENGINE_UNUSED_PARAM(funcName);
    bool bRet = false;
    if (nullptr != L && nullptr != outValue)
    {
        tolua_Error tolua_err;
        bRet = tolua_istable(L, lo, 0, &tolua_err) != 0;
        if (bRet)
        {
            size_t len = lua_objlen(L, lo);
            bRet = len == 16;
            BEATS_ASSERT(bRet, _T("matrix should contain 16 float(double)"));
            if (bRet)
            {
                for (size_t i = 0; i < len; i++)
                {
                    lua_pushnumber(L, i + 1);
                    lua_gettable(L, lo);
                    if (tolua_isnumber(L, -1, 0, &tolua_err))
                    {
                        (*outValue)[i] = (float)tolua_tonumber(L, -1, 0);
                    }
                    else
                    {
                        (*outValue)[i] = 0;
                    }
                    lua_pop(L, 1);
                }
            }
        }
#ifdef _DEBUG
        else
        {
            luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
        }
#endif
    }
    return bRet;
}

template <class TKey, class TValue>
inline bool GetValueFromLua(lua_State* L, int lo, std::map<TKey, TValue>& outValue, const char* funcName)
{
    bool bRet = false;
    if (nullptr != L && lua_gettop(L) >= lo)
    {
        tolua_Error tolua_err;
        bRet = tolua_istable(L, lo, 0, &tolua_err);
        if (bRet)
        {
            lua_pushnil(L);

            while (lua_next(L, lo) != 0)
            {
                TKey keyval;
                TValue val;
                GetValueFromLua(L, lo, &keyval, "get map key");
                GetValueFromLua(L, lo, &val, "get map value");
                outValue[keyval] = val;
                lua_pop(L, 1);
            }
        }
#ifdef _DEBUG
        else
        {
            luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
        }
#endif
    }
    return bRet;
}

// Send value to lua
template <class T>
inline void SendValueToLua(lua_State* , const T )
{
    BEATS_ASSERT(false, "can not reach here");
}

#define DECLARE_SEND_NUMBER_TO_LUA_TYPE(Type) \
template <>\
inline void SendValueToLua(lua_State* L, const Type value)\
{\
    tolua_pushnumber(L, lua_Number(value));\
}

DECLARE_SEND_NUMBER_TO_LUA_TYPE(short)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(unsigned short)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(char)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(unsigned char)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(float)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(double)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(long)
DECLARE_SEND_NUMBER_TO_LUA_TYPE(unsigned long)

template <>
inline void SendValueToLua(lua_State* L, bool value)
{
    tolua_pushboolean(L, value);
}

template <>
inline void SendValueToLua(lua_State* L, const char* value)
{
    tolua_pushstring(L, value);
}

template <>
inline void SendValueToLua(lua_State* L,  std::string value)
{
    tolua_pushstring(L, value.c_str());
}

template <>
inline void SendValueToLua(lua_State* L,  CVec2 value)
{
    if (nullptr != L)
    {
        lua_newtable(L);                                    /* L: table */
        lua_pushstring(L, "x");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.X());               /* L: table key value*/
        lua_rawset(L, -3);                                  /* table[key] = value, L: table */
        lua_pushstring(L, "y");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.Y());               /* L: table key value*/
        lua_rawset(L, -3);
    }
}

template <>
inline void SendValueToLua(lua_State* L,  CVec3 value)
{
    if (nullptr != L)
    {
        lua_newtable(L);                                    /* L: table */
        lua_pushstring(L, "x");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.X());             /* L: table key value*/
        lua_rawset(L, -3);                                  /* table[key] = value, L: table */
        lua_pushstring(L, "y");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.Y());             /* L: table key value*/
        lua_rawset(L, -3);
        lua_pushstring(L, "z");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.Z());             /* L: table key value*/
        lua_rawset(L, -3);
    }
}

template <>
inline void SendValueToLua(lua_State* L,  CVec4 value)
{
    if (nullptr != L)
    {
        lua_newtable(L);                                    /* L: table */
        lua_pushstring(L, "x");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.X());             /* L: table key value*/
        lua_rawset(L, -3);                                  /* table[key] = value, L: table */
        lua_pushstring(L, "y");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.Y());             /* L: table key value*/
        lua_rawset(L, -3);
        lua_pushstring(L, "z");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.Z());             /* L: table key value*/
        lua_rawset(L, -3);
        lua_pushstring(L, "w");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.W());             /* L: table key value*/
        lua_rawset(L, -3);
    }
}

template <>
inline void SendValueToLua(lua_State* L,  CColor value)
{
    if (nullptr != L)
    {
        lua_newtable(L);                                    /* L: table */
        lua_pushstring(L, "r");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.r);             /* L: table key value*/
        lua_rawset(L, -3);                                  /* table[key] = value, L: table */
        lua_pushstring(L, "g");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.g);             /* L: table key value*/
        lua_rawset(L, -3);
        lua_pushstring(L, "b");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.b);             /* L: table key value*/
        lua_rawset(L, -3);
        lua_pushstring(L, "a");                             /* L: table key */
        lua_pushnumber(L, (lua_Number)value.a);             /* L: table key value*/
        lua_rawset(L, -3);
    }
}

template <>
inline void SendValueToLua(lua_State* L,  CMat4 value)
{
    if (nullptr != L)
    {
        lua_newtable(L);                                    /* L: table */
        int indexTable = 1;

        for (int i = 0; i < 16; i++)
        {
            lua_pushnumber(L, (lua_Number)indexTable);
            lua_pushnumber(L, (lua_Number)value[i]);
            lua_rawset(L, -3);
            ++indexTable;
        }
    }
}

template <class T>
inline void SendValueToLua(lua_State* L,  std::vector<T> value)
{
    if (nullptr != L)
    {
        lua_newtable(L);
        int indexTable = 1;
        for (size_t i = 0; i < value.size(); ++i)
        {
            lua_pushnumber(L, (lua_Number)indexTable);
            SendValueToLua(L, value[i]);
            lua_rawset(L, -3);
            ++indexTable;
        }
    }
}

template <class TKey, class TValue>
inline void SendValueToLua(lua_State* L,  std::map<TKey, TValue> value)
{
    if (L != nullptr)
    {
        lua_newtable(L);
        for (auto iter = value.begin(); iter != value.end(); ++iter)
        {
            SendValueToLua(L, iter->first);
            SendValueToLua(L, iter->second);
            lua_rawset(L, -3);
        }
    }
}
#endif
