#set varCount = len($fields)
#set currCount = 0;
#while $currCount < $varCount
#set fieldName = $fields[$currCount].name
#set fieldType = $fields[$currCount].type
int lua_${namespaced_class_name}_${fieldName}_setter(lua_State* tolua_S)
{
    $fieldType value;
    GetValueFromLua(tolua_S, -1, &value, "field setter");
    ${namespaced_class_name}* pObject = (${namespaced_class_name}*)tolua_tousertype(tolua_S,-2,0);
    BEATS_ASSERT(pObject != nullptr);
    pObject->${fieldName} = value;
    return 1;
}

int lua_$(namespaced_class_name)_${fieldName}_getter(lua_State* tolua_S)
{
    ${namespaced_class_name}* pObject = (${namespaced_class_name}*)tolua_tousertype(tolua_S,-1,0);
    BEATS_ASSERT(pObject != nullptr);
    SendValueToLua(tolua_S, pObject->${fieldName});
    return 1;
}
#set $currCount = $currCount + 1
#end while