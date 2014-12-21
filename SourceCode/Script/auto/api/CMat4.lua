
--------------------------------
-- @module CMat4
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#CMat4] PerspectiveProjection 
-- @param self
-- @param #float fovDegree
-- @param #float aspect
-- @param #float zNear
-- @param #float zFar
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] Inverse 
-- @param self
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] FromPitchYawRoll 
-- @param self
-- @param #float pitch
-- @param #float yaw
-- @param #float roll
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- @overload self, CVec3         
-- @overload self, CMat4         
-- @function [parent=#CMat4] operator*
-- @param self
-- @param #CMat4 rhs
-- @return CMat4#CMat4 ret (return value: CMat4)

--------------------------------
-- 
-- @function [parent=#CMat4] IsIdentity 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- @overload self, CVec3, CVec3, CVec3         
-- @overload self, CVec3, CQuaternion, CVec3         
-- @function [parent=#CMat4] Build
-- @param self
-- @param #CVec3 translate
-- @param #CQuaternion rotate
-- @param #CVec3 scale
-- @return CMat4#CMat4 self (return value: CMat4)

--------------------------------
-- 
-- @function [parent=#CMat4] FromAxisAngle 
-- @param self
-- @param #CVec3 axis
-- @param #float radians
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] RemoveTranslate 
-- @param self
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] OrthographicProjection 
-- @param self
-- @param #float left
-- @param #float right
-- @param #float bottom
-- @param #float top
-- @param #float nearVal
-- @param #float farVal
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] Mat4ValuePtr 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#CMat4] LookAt 
-- @param self
-- @param #CVec3 eye
-- @param #CVec3 center
-- @param #CVec3 up
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] FromQuaternion 
-- @param self
-- @param #CQuaternion pQ
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] SetTranslate 
-- @param self
-- @param #CVec3 translate
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] ToAxisAngle 
-- @param self
-- @param #CVec3 pAxis
-- @param #float radians
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] operator!= 
-- @param self
-- @param #CMat4 rhs
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CMat4] GetTranslate 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CMat4] GetRightVec3 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CMat4] operator== 
-- @param self
-- @param #CMat4 rhs
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CMat4] operator[] 
-- @param self
-- @param #unsigned int uIndex
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#CMat4] Decompose 
-- @param self
-- @param #CVec3 translate
-- @param #CQuaternion rotate
-- @param #CVec3 scale
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CMat4] GetUpVec3 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CMat4] RemoveScale 
-- @param self
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] GetForwardVec3 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
--------------------------------
-- 
-- @function [parent=#CMat4] Identity 
-- @param self
-- @return CMat4#CMat4 self (return value: CMat4)
        
--------------------------------
-- 
-- @function [parent=#CMat4] GetScale 
-- @param self
-- @return CVec3#CVec3 ret (return value: CVec3)
        
