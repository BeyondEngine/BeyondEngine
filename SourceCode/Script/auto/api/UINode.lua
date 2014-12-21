
--------------------------------
-- @module UINode
-- @extend CNode
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#UINode] findNodeByTargetName 
-- @param self
-- @param #string name
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getRotation 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getUIChildren 
-- @param self
-- @return array_table#array_table ret (return value: array_table)
        
--------------------------------
-- 
-- @function [parent=#UINode] getRotateY 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getRotateX 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getTouchEnable 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] getRotateZ 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] setContentSize 
-- @param self
-- @param #float widht
-- @param #float height
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] removeAllChild 
-- @param self
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] AddChild 
-- @param self
-- @param #UINode node
-- @param #int zOrder
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getScaleZ 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getScaleY 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getScaleX 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] GetClassStr 
-- @param self
-- @return char#char ret (return value: char)
        
--------------------------------
-- 
-- @function [parent=#UINode] getRelativeType 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#UINode] setAlpha 
-- @param self
-- @param #float alpha
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getContentSize 
-- @param self
-- @return CVec2#CVec2 ret (return value: CVec2)
        
--------------------------------
-- 
-- @function [parent=#UINode] setRotation 
-- @param self
-- @param #float angle
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] setLuaFile 
-- @param self
-- @param #string file
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] setScreenRect 
-- @param self
-- @param #CVec2 pos
-- @param #CVec2 size
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getDisPlayAlpha 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] canBeEditAsRoot 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] setNeedCheckContentSizeInTouch 
-- @param self
-- @param #bool need
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getZorder 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#UINode] removeFromParent 
-- @param self
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] convertToNodeSpace 
-- @param self
-- @param #CVec3 worldPoint
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#UINode] GetAnchor 
-- @param self
-- @return CVec2#CVec2 ret (return value: CVec2)
        
--------------------------------
-- 
-- @function [parent=#UINode] checkPointInContentSize 
-- @param self
-- @param #CVec2 point
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] setZorder 
-- @param self
-- @param #int z
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] RemoveChild 
-- @param self
-- @param #CNode node
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] checkPointInContentSizeWithSkip 
-- @param self
-- @param #CVec2 point
-- @param #float skip
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] enableUpdate 
-- @param self
-- @param #bool enable
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] updataEditorShowValue 
-- @param self
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getPositionZ 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getPositionY 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getPositionX 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] getUINodePosition 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#UINode] isNeedCheckContentSizeInTouch 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#UINode] getParentUINode 
-- @param self
-- @return UINode#UINode ret (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] SetAnchor 
-- @param self
-- @param #CVec2 anchor
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] getClipRect 
-- @param self
-- @return CRect#CRect ret (return value: CRect)
        
--------------------------------
-- 
-- @function [parent=#UINode] getAlpha 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#UINode] setRelativeType 
-- @param self
-- @param #int type
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] setTouchEnable 
-- @param self
-- @param #bool enable
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- 
-- @function [parent=#UINode] GetWorldPosition 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#UINode] GetParentNode 
-- @param self
-- @return CNode#CNode ret (return value: CNode)
        
--------------------------------
-- 
-- @function [parent=#UINode] InvalidateWorldTM 
-- @param self
-- @return UINode#UINode self (return value: UINode)
        
--------------------------------
-- @overload self, CVec3         
-- @overload self, float, float, float         
-- @function [parent=#UINode] SetPosition
-- @param self
-- @param #float x
-- @param #float y
-- @param #float z
-- @return bool#bool ret (return value: bool)

--------------------------------
-- 
-- @function [parent=#UINode] UINode 
-- @param self
-- @return UINode#UINode self (return value: UINode)
        
return nil
