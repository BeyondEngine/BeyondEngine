
__G__TRACKBACK__ = function(msg)
    local msg = debug.traceback(msg, 3)
    print(msg)
    return msg
end

InitScriptPath = function()
    local um = CUISystemManager:GetInstance()
    local scripPath = um:GetResourcePath("script")
	package.path = package.path..';'..scripPath..'/?.lua'
end

LoadBcfFile = function(fileName)
    local um = CUISystemManager:GetInstance()
    um:LoadUIBcf(fileName)
end

AddChildToRoot = function(node)
	local um = CUISystemManager:GetInstance()
	local root = um:GetRootNode()
	root:AddUIChild(node)
end

AddChildToRootIfNoParent = function(node)
    local parentNode = node:GetParentUINode()
	if parentNode == nil then
		AddChildToRoot(node)
	end
end

getUINodeByName = function(name)
local um = CUISystemManager:GetInstance()
local node = um:GetUINodeByName(name)
return node
end

getResourcePath = function(name)
local um = CUISystemManager:GetInstance()
local path = um:GetResourcePath(name)
return path;
end

RegUINodeEventCallback = function(nodeName, eventId, func)
	Lua_RegEventCallback( nodeName, eventId , func)
end

function print_r(sth)
	if type(sth) ~= "table" then
		print(sth)
		return
	end

	local space, deep = string.rep(' ', 4), 0
	local function _dump(t)
	local temp = {}
	for k,v in pairs(t) 
		do
			local key = tostring(k)
	
			if type(v) == "table" then
				deep = deep + 2
				print(string.format("%s[%s] => Table\n%s(",string.rep(space, deep - 1),key, string.rep(space, deep))) --print.
				_dump(v)
				print(string.format("%s)",string.rep(space, deep)))
				deep = deep - 2
			else
				print(string.format("%s[%s] => %s", string.rep(space, deep + 1), key, v)) --print.
			end 
		end 
	end

	print(string.format("Table\n("))
	_dump(sth)
	print(string.format(")"))
end