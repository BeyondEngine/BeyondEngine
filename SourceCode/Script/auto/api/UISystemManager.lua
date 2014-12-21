
--------------------------------
-- @module UISystemManager
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#UISystemManager] setMouseMovePos 
-- @param self
-- @param #float x
-- @param #float y
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] registerUpdateNode 
-- @param self
-- @param #UINode node
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] setUIModel 
-- @param self
-- @param #int model
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] findNodeByTargetName 
-- @param self
-- @param #string name
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getTreeSelectedNode 
-- @param self
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] loadUIBcf 
-- @param self
-- @param #string path
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] recycleNodeForEditorModel 
-- @param self
-- @param #UINode source
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getDrawNode 
-- @param self
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getRootNode 
-- @param self
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] cleanUI 
-- @param self
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] isInEditorModel 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] setDrawNode 
-- @param self
-- @param #UINode drawNode
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getResourcePath 
-- @param self
-- @param #string file
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] isBcfFileLoad 
-- @param self
-- @param #int fileId
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] allUINodeSetNamed 
-- @param self
-- @param #UINode node
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] Initialize 
-- @param self
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] selectedUINodeFromTree 
-- @param self
-- @param #UINode node
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] addLuaFile 
-- @param self
-- @param #string file
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getUISelectedNode 
-- @param self
-- @return UISelectedNode#UISelectedNode ret (return value: UISelectedNode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] checkNameExisted 
-- @param self
-- @param #UINode node
-- @param #string name
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] changeUINodeName 
-- @param self
-- @param #UINode node
-- @param #string name
-- @param #bool needUpdate
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getEditUINode 
-- @param self
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] unRegisterUpdateNode 
-- @param self
-- @param #UINode node
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] getScreenSize 
-- @param self
-- @return CVec2#CVec2 ret (return value: CVec2)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] saveUIBcfFile 
-- @param self
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] initOtherBfcAndLuaFiles 
-- @param self
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] isRootComponent 
-- @param self
-- @param #CComponentProxy pComponent
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] replaceNodeForEditorModel 
-- @param self
-- @param #UINode source
-- @param #UINode target
-- @param #bool isCrtlPressed
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] unLoadUIBcf 
-- @param self
-- @param #string path
-- @return UISystemManager#UISystemManager self (return value: UISystemManager)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] HasInstance 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UISystemManager] GetInstance 
-- @param self
-- @return UISystemManager#UISystemManager ret (return value: UISystemManager)
        
return nil
