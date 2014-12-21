
--------------------------------
-- @module CNode
-- @extend CComponentInstance
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#CNode] SetColorScale 
-- @param self
-- @param #CColor color
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] Deactivate 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] Update 
-- @param self
-- @param #float dtt
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] HitTest 
-- @param self
-- @param #CVec2 pt
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] RemoveChild 
-- @param self
-- @param #CNode pNode
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] NodeAnimationUpdate 
-- @param self
-- @param #int type
-- @param #unsigned int uCurrFrame
-- @param #CVec3 currValue
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] IsVisible 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetVisible 
-- @param self
-- @param #bool bVisible
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] AddChild 
-- @param self
-- @param #CNode pChild
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetWorldPosition 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetParentNode 
-- @param self
-- @return CNode#CNode ret (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetScale 
-- @param self
-- @param #CVec3 scale
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetChildren 
-- @param self
-- @return array_table#array_table ret (return value: array_table)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetRootNode 
-- @param self
-- @return CNode#CNode ret (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetRotation 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetLocalTM 
-- @param self
-- @param #CMat4 localTM
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] Render 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] InvalidateWorldTM 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetChildByName 
-- @param self
-- @param #string name
-- @return CNode#CNode ret (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetName 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#CNode] InvalidateLocalTM 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetRotation 
-- @param self
-- @param #CVec3 rotation
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetPosition 
-- @param self
-- @param #CVec3 position
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] Activate 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetColorScale 
-- @param self
-- @param #bool bInherit
-- @return CColor#CColor ret (return value: CColor)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetName 
-- @param self
-- @param #string strName
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] SetParentNode 
-- @param self
-- @param #CNode pParent
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetType 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#CNode] IsActive 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetScale 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CNode] Clone 
-- @param self
-- @param #bool 
-- @param #CSerializer pSerializer
-- @param #unsigned int id
-- @param #bool bCallInitFunc
-- @return CComponentBase#CComponentBase ret (return value: CComponentBase)
        
--------------------------------
-- 
-- @function [parent=#CNode] GetPosition 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CNode] Initialize 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] Uninitialize 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#CNode] CNode 
-- @param self
-- @return CNode#CNode self (return value: CNode)
        
return nil
