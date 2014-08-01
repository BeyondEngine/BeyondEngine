/**********************************************************************
*<
FILE:			IViewportShadingMgr.h

DESCRIPTION:	Core interface to control Viewport Shading, including Shadows

CREATED BY:		Neil Hazzard

HISTORY:		created March 8th, 2007

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/

#ifndef __IVIEWPORTSHADINGMGR__H__
#define __IVIEWPORTSHADINGMGR__H__

#include "iFnPub.h"

#define IVIEWPORTSHADINGMGR_INTERFACE Interface_ID(0x7eba45d1, 0x2b3d454b)

//! Viewport manager to control general illumination, including shadow support in the viewport
/*! New to 3ds max 2008, the IViewportShadingMgr gives access to the new Viewport Shading feature.
The viewports have be redesigned to support more realistic rendering including Shadows.  The interface also 
provides access to various flags that control how lights are used in the viewport.  This is used closely with 
the new INodeShadingProperites API, that manages the actual flags set by IViewportShadingMgr
The developer can get access to the CORE interface by used
\code
IViewportShadingMgr * pShaderMgr = GetIViewportShadingMgr();
\endcode

*/
class IViewportShadingMgr : public FPStaticInterface
{

public:
	enum
	{
		kNone,
		kGood,
		kBest,
	};

	enum{
		kShadowCasting,
		kIlluminating,
		kLocked,
	};
	
	//! Define the viewport Quality
	/*! Set the viewport rendering quality to None, Good or Best.  Depending 
	on the node flags, the viewport will display Per pixel lighting and shadowing.  Please make sure you call
	IsShadingLimitValid before this method to determine if the graphics hardware supports this shading level.
	\param limit The shading limits as defined by the enum
	\param bQuiet True to enable quiet mode.  No dialogs will be produced to provide user feedback.  This 
	defaults to true.
	*/
	virtual void SetViewportShadingLimits(int limit, bool bQuiet = true)=0;

	//! Retrieve the current Viewport Quality
	/*! Get the current viewport rendering quality, as defined by the enum
	\returns The current Shading limits
	*/
	virtual int GetViewportShadingLimits()=0;

	//! Get the current state of the Auto Display Selected light flag
	/*! This will return the current state of the Auto Display of selected lights flag
	Any lights that are selected will automatically have their illumination flag set and
	take part in viewport rendering.  If they have their shadow flag set as well, they will 
	become shadow casters.
	\return The state of the flag
	*/
	virtual bool GetAutoDisplayOfSelLights()=0;

	//! Set the state of the Auto Display of Selected lights falg
	/*! This will set the current state of the Auto Display of selected lights flag
	If set to true, any lights that are selected will automatically have their illumination flag set and
	take part in viewport rendering.  If they have their shadow flag set as well, they will 
	become shadow casters.
	\param bSet The flag value
	*/
	virtual void SetAutoDisplayOfSelLights(bool bSet)=0;

	//! Flag controlling shadow generation for default lights
	/*! Defines whether default lights cast shadows in the viewport
	\param bSet The flag value
	*/
	virtual void SetCastShadowDefaultLights(bool bSet)=0;

	//! Access the flag defining whether shadows are generated for default lights
	/*!
	\return The state of the flag
	*/
	virtual bool GetCastShadowDefaultLights()=0;

	//! A method that defines that the current selected lights will be part of viewport rendering
	/*! When used, all lights that are currently selected will have their Illumination flag set, and will
	be part of the viewport rendering.  All unselected lights will be deactivated.
	*/
	virtual void DisplayOnlySelectedLights()=0;

	//! Lock the current selected lights
	/*! A locked light will be active in the viewport rendering, regardless of other flags set
	This could be something like a sun light, that always illuminates and casts shadows.  Any selected
	light will have this flag set depending on the parameter value
	\param The value of the flag to set.
	*/
	virtual void LockSelectedLights(bool bSet)=0;

	//! A method to retrieve lights based on the usage in the scene.
	/*! This is a general method to retrieve a list of nodes based on the property.  This is 
	a quick way off accessing all Viewport shadow casting lights in the scene.
	\param &lightList  A node tab to retrieve the node list based on type
	\param type The light type to store, either Shadow Casting, Illumination or Locked
	*/
	virtual void GetViewportShadingLights(Tab<INode*> &lightList, int type) = 0;

	//! Sets the global intensity override for the shadows
	/*! The intensity value is a global overide for the general appearence of the shadow.  A value
	of 1.0, the default will provide the unaltered shadow.  A value less than one will fade the shadow
	to produce different effects.
	\param intensity The value to set the intensity.  This should be between 0 and 1, but the function will clamp
	*/
	virtual void SetGlobalShadowIntensity(float intensity) = 0;

	//! Retreive the Global shadow intensity
	/*! This will return the current intensity value for the shadows.
	\returns The intensity value
	*/
	virtual float GetGlobalShadowIntensity()=0;

	//! Defines whether the viewport shading code supports transparent object/materials in the shadow code
	/*! This is a performance flag.  If it is turned off, then the viewport will become faster as less GPU 
	resources are active.  This flag is typically used by the Adaptive Degradation system
	\param bSet True to turn it on - False to turn it off
	*/
	virtual void SetUseTransparencyInShadows(bool bSet) = 0;

	//! Get the current Transparency in shadows flag
	/*! See SetUseTransparencyInShadows
	\returns The current state of the flag
	*/
	virtual bool GetUseTransparencyInShadows() = 0;

	//! Defines whether the viewport shading code updates the shadows for all windows or just the active viewport
	/*! The us a performance flag.  TO increase performance in the viewports, turning this flag on will only update
	shadows in the active viewport - if it is a supported mode.
	\param bSet True to turn it on - False to turn it off
	*/
	virtual void SetUpdateShadowInActiveViewOnly(bool bSet) =0;

	//!Get the current update shadow in active view only flag
	/*! See SetUpdateShadowInActiveViewOnly
	\returns The current state of the flag
	*/
	virtual bool GetUpdateShadowInActiveViewOnly()=0;

	//! Determine whether the shading limit is compatable with the current graphics hardware 
	/*! This method is used to determine if the graphics hardware can support the shading level.  This 
	will query the GPU to find out what pixel shader support exists.  For "Best", shader model 3 is required,
	"Good" needs a minimum of shader model 2.  The developer should call this method before calling 
	SetViewportShadingLimits as that method will not perform any additional tests.
	\param limit The shading limit to test
	\return True is the shading limit is supported
	*/
	virtual bool IsShadingLimitValid(int limit)=0;

};


inline IViewportShadingMgr* GetIViewportShadingMgr() {

	return static_cast<IViewportShadingMgr*>(GetCOREInterface(IVIEWPORTSHADINGMGR_INTERFACE));
};

#endif