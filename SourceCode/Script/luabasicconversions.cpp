#include "stdafx.h"
#include "luabasicconversions.h"

std::unordered_map<std::string, std::string>  g_luaType;
std::unordered_map<std::string, std::string>  g_typeCast;

void luaval_to_native_err(lua_State* L,const char* msg,tolua_Error* err, const char* funcName)
{
    BEYONDENGINE_UNUSED_PARAM(L);
    BEYONDENGINE_UNUSED_PARAM(msg);
    BEYONDENGINE_UNUSED_PARAM(err);
    BEYONDENGINE_UNUSED_PARAM(funcName);
#ifdef _DEBUG
    if (NULL == L || NULL == err || NULL == msg || 0 == strlen(msg))
        return;

    if (msg[0] == '#')
    {
        const char* expected = err->type;
        const char* provided = tolua_typename(L,err->index);
        if (msg[1]=='f')
        {
            int narg = err->index;
            BEATS_PRINT("%s\n     %s argument #%d is %s '%s'; array of '%s' expected.\n", msg + 2, funcName, narg, provided, (err->array ? "array of" : ""), expected);
        }
        else if (msg[1]=='v')
        {
            BEATS_PRINT("%s\n     %s value is %s '%s'; array of '%s' expected.\n", funcName, msg + 2, (err->array ? "array of" : ""), provided, expected);
        }
    }
#endif
}

#ifdef __cplusplus
extern "C" {
#endif
extern int lua_isusertype (lua_State* L, int lo, const char* type);
#ifdef __cplusplus
}
#endif

bool luaval_is_usertype(lua_State* L,int lo,const char* type, int def)
{
    return (def && lua_gettop(L) < abs(lo)) || (lua_isnil(L, lo) || lua_isusertype(L, lo, type));
}