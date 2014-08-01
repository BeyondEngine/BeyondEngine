/*	
 *		iParamWire.h - Public interface to Parameter Wiring Manager & Wire Controllers
 *
 *			Copyright © Autodesk, Inc, 2000.  John Wainwright.
 *
 */

#ifndef _H_IPARAMWIRE
#define _H_IPARAMWIRE

#ifndef NO_PARAMETER_WIRING	// russom - 02/14/02

#include "iFnPub.h"

class IParamWireMgr;
class IBaseWireControl;

// ---------  parameter wiring manager, provides gerenal access to param wiring functions ---

#define PARAMWIRE_MGR_INTERFACE   Interface_ID(0x490d0e99, 0xbe87c96)
inline IParamWireMgr* GetParamWireMgr() { return (IParamWireMgr*)GetCOREInterface(PARAMWIRE_MGR_INTERFACE); }

#define PWMF_LEFT_TARGET	0x001
#define PWMF_RIGHT_TARGET	0x002
#define PWMF_HAS_MENU		0x004
#define PWMF_OPEN_EDITOR	0x008

// class IParamWireMgr
//    parameter wiring manager interface 
/*! \sa  Class FPStaticInterface,  Class Control, Class ReferenceTarget\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class represents the interface that provides general access to the
parameter wiring functions. You can obtain a pointer to the Parameter Wire
Manager interface using; <b>IParamWireMgr* GetParamWireMgr()</b>. This macro
will return
<b>(IParamWireMgr*)GetCOREInterface(PARAMWIRE_MGR_INTERFACE).</b>\n\n
All methods of this class are Implemented by the System.  */
class IParamWireMgr : public FPStaticInterface 
{
public:
	// function IDs 
	enum { startParamWire,
		   openEditor,
		   editParams, editParam,
		   editControllers, editController,
		   connect, connect2Way,
		   disconnect, disconnect2Way
		}; 

	/*! \remarks This method will launch the parameter wiring UI mode. */
	virtual void StartParamWire()=0;					// launch param wiring UI mode
	/*! \remarks This method will open up the parameter wiring dialog on the
	selected objects. */
	virtual void OpenEditor()=0;						// open param wiring dialog on selected objects
	/*! \remarks This method allows you to edit the left- and right-hand
	parameters and opens the parameter wiring dialog using the provided
	parameters.
	\par Parameters:
	<b>ReferenceTarget* leftParent</b>\n\n
	A pointer to the left-hand reference target.\n\n
	<b>int leftSubNum</b>\n\n
	The sub-animatable of the left-hand reference target.\n\n
	<b>ReferenceTarget* rightParent</b>\n\n
	A pointer to the right-hand reference target.\n\n
	<b>int rightSubNum</b>\n\n
	The sub-animatable of the right-hand reference target. */
	virtual void EditParams(ReferenceTarget* leftParent, int leftSubNum,
							ReferenceTarget* rightParent, int rightSubNum)=0;  // edit params
	virtual void EditParam(ReferenceTarget* parent, int subNum)=0;
	/*! \remarks This method allows you to setup the two controllers for the
	left- and right-hand to edit.
	\par Parameters:
	<b>Control* leftWire</b>\n\n
	A pointer to the controller for the left-hand wire.\n\n
	<b>Control* leftWire</b>\n\n
	A pointer to the controller for the right-hand wire. */
	virtual void EditControllers(Control* leftWire, Control* rightWire)=0;
	/*! \remarks This method is identical to the <b>EditControllers()</b> but
	accepts a single wire controller for the left-hand. This method effectively
	calls <b>EditControllers(wire, NULL)</b>.
	\par Parameters:
	<b>Control* wire</b>\n\n
	A pointer to the controller being edited. */
	virtual void EditController(Control* wire)=0;
	/*! \remarks This method allows you to set up a one-way wire.
	\par Parameters:
	<b>ReferenceTarget* fromParent</b>\n\n
	A pointer to the reference target to wire from.\n\n
	<b>int fromSubNum</b>\n\n
	The sub-animatable to wire from.\n\n
	<b>ReferenceTarget* toParent</b>\n\n
	A pointer to the reference target to wire to.\n\n
	<b>int toSubNum</b>\n\n
	The sub-animatable to wire to.\n\n
	<b>MCHAR* toExpr</b>\n\n
	A string containing the expression on the "to wire".
	\return  TRUE if the connection can be made, otherwise FALSE. */
	virtual bool Connect(ReferenceTarget* fromParent, int fromSubNum,
							ReferenceTarget* toParent, int toSubNum,
							TCHAR* toExpr)=0;		 // set up a one-way wire from -> to
	/*! \remarks This method allows you to set up a two-way wire.
	\par Parameters:
	<b>ReferenceTarget* leftParent</b>\n\n
	A pointer to the left-hand reference target.\n\n
	<b>int leftSubNum</b>\n\n
	The sub-animatable of the left-hand reference target.\n\n
	<b>ReferenceTarget* rightParent</b>\n\n
	A pointer to the right-hand reference target.\n\n
	<b>int rightSubNum</b>\n\n
	The sub-animatable of the right-hand reference target.\n\n
	<b>MCHAR* leftExpr</b>\n\n
	A string containing the expression for the left-hand target.\n\n
	<b>MCHAR* rightExpr = NULL</b>\n\n
	A string containing the expression for the right-hand target.
	\return  TRUE if the connection can be made, otherwise FALSE. */
	virtual bool Connect2Way(ReferenceTarget* leftParent, int leftSubNum,
							ReferenceTarget* rightParent, int rightSubNum,
							TCHAR* leftExpr, TCHAR* rightExpr=NULL)=0;     // set up a two-way wire
	/*! \remarks This method allows you to disconnect a one-way wire.
	\par Parameters:
	<b>Control* wireController</b>\n\n
	A pointer to the wire controller you wish to disconnect.
	\return  TRUE if the disconnect was successful, otherwise FALSE. */
	virtual bool Disconnect(Control* wireController)=0;  // disconnect one-way
	/*! \remarks This method allows you to disconnect a two-way wire.
	\par Parameters:
	<b>Control* wireController1</b>\n\n
	A pointer to the first wire controller you wish to disconnect.\n\n
	<b>Control* wireController2</b>\n\n
	A pointer to the second wire controller you wish to disconnect.
	\return  TRUE if the disconnect was successful, otherwise FALSE. */
	virtual bool Disconnect2Way(Control* wireController1, Control* wireController2)=0;  // disconnect two-way

	virtual Animatable* ParamWireMenu( ReferenceTarget* pTarget, int iSubNum, int iFlags = PWMF_LEFT_TARGET, HWND hWnd = NULL, IPoint2 *pPt = NULL )=0;
}; 

// ------ individual wire controller interface -------------

// wire controller classes & names

#define FLOAT_WIRE_CONTROL_CLASS_ID		Class_ID(0x498702e7, 0x71f11549)
#define POSITION_WIRE_CONTROL_CLASS_ID	Class_ID(0x5065767c, 0x683a42a6)
#define POINT3_WIRE_CONTROL_CLASS_ID	Class_ID(0x4697286a, 0x2f7f05cf)
#define POINT4_WIRE_CONTROL_CLASS_ID	Class_ID(0x4697286b, 0x2f7f05ff)
#define ROTATION_WIRE_CONTROL_CLASS_ID	Class_ID(0x31381913, 0x3a904167)
#define SCALE_WIRE_CONTROL_CLASS_ID		Class_ID(0x7c8f3a2b, 0x1e954d92)

#define WIRE_CONTROLLER_INTERFACE   Interface_ID(0x25ce0f5c, 0x6c303d2f)
inline IBaseWireControl* GetWireControlInterface(Animatable* a) { return (IBaseWireControl*)a->GetInterface(WIRE_CONTROLLER_INTERFACE); }

/*! \sa  Class Control, Class StdControl, Class FPMixinInterface\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class represents the interface for individual wire controllers. You can
obtain a pointer to the Base Wire control interface using; <b>IBaseWireControl*
GetWireControlInterface(Animatable* a)</b>. This macro will return
<b>(IBaseWireControl*)a-\>GetInterface(WIRE_CONTROLLER_INTERFACE).</b>\n\n
The Class ID's of individual wire controllers are;
<b>FLOAT_WIRE_CONTROL_CLASS_ID, POSITION_WIRE_CONTROL_CLASS_ID,
POINT3_WIRE_CONTROL_CLASS_ID, ROTATION_WIRE_CONTROL_CLASS_ID,
SCALE_WIRE_CONTROL_CLASS_ID</b>\n\n
All methods of this class are Implemented by the System.  */
class IBaseWireControl : public StdControl, public FPMixinInterface {
	public:
		// local 
		// number of wires, wire param access
		/*! \remarks This method returns the number of wires out of this
		controller (i.e. the number of dependent params). */
		virtual int		get_num_wires()=0;			 // how many wires out of this controller (number of dependent params)
		/*! \remarks This method returns a pointer to the i-th dependent
		parameter parent.
		\par Parameters:
		<b>int i</b>\n\n
		The index you wish to retrieve. */
		virtual Animatable* get_wire_parent(int i)=0;  // get ith dependent parameter parent animatable
		/*! \remarks This method returns the i-th dependent parameter subanim
		num in the animatable.
		\par Parameters:
		<b>int i</b>\n\n
		The index of the subanim. */
		virtual int		get_wire_subnum(int i)=0;    // get ith dependent parameter subanim num in the animatable
		/*! \remarks This method returns a pointer to the i-th CoController.
		\par Parameters:
		<b>int i</b>\n\n
		The index of the controller. */
		virtual Control* get_co_controller(int i)=0; // get ith co_controller

		// transfer expression script
		/*! \remarks This method returns the expression string of the i-th
		wire parameter.
		\par Parameters:
		<b>int i</b>\n\n
		The index of the parameter. */
		virtual TCHAR*	get_expr_text(int i)=0;
		/*! \remarks This method allows you to set the expression string of
		the i-th wire parameter.
		\par Parameters:
		<b>int i</b>\n\n
		The index of the parameter\n\n
		<b>MCHAR* text</b>\n\n
		The expression you wish to set. */
		virtual void	set_expr_text(int i, TCHAR* text)=0;

		// animation sub-controller
		/*! \remarks This method allows you to set the slave animation
		controller.
		\par Parameters:
		<b>Control* c</b>\n\n
		A pointer to the controller you wish to set. */
		virtual void	set_slave_animation(Control* c)=0;
		/*! \remarks This method returns a pointer to the slave animation
		controller. */
		virtual Control* get_slave_animation()=0;

		// type predicate
		/*! \remarks This method will return TRUE if the wire is a master
		predicate, otherwise it will return FALSE. */
		virtual bool    is_master()=0;
		/*! \remarks This method will return TRUE if the wire is a slave
		predicate, otherwise it will return FALSE. */
		virtual bool	is_slave()=0;
		/*! \remarks This method will return TRUE if the wire is a two-way
		predicate, otherwise it will return FALSE. */
		virtual bool	is_two_way()=0;

		// parent/subnum transfers
		virtual void transfer_parent(ReferenceTarget* oldp, ReferenceTarget* newp)=0;
		virtual void transfer_subnum(short oldn, short newn)=0;

		// FnPub stuff
		enum { getNumWires, getWireParent, getWireSubnum, getCoController, 
			   getExprText, setExprText, 
			   getSlaveAnimation, setSlaveAnimation, isMaster, isSlave, isTwoWay, };

		// from FPInterface
		FPInterfaceDesc* GetDesc() { return GetDescByID(WIRE_CONTROLLER_INTERFACE); }

		BEGIN_FUNCTION_MAP
			FN_1(getWireParent,		 TYPE_REFTARG,	 get_wire_parent,	TYPE_INDEX);
			FN_1(getWireSubnum,		 TYPE_INDEX,	 get_wire_subnum,	TYPE_INDEX);
			FN_1(getCoController,	 TYPE_CONTROL,	 get_co_controller,	TYPE_INDEX);
			FN_1(getExprText,		 TYPE_STRING,	 get_expr_text,		TYPE_INDEX);
			VFN_2(setExprText,						 set_expr_text,		TYPE_INDEX, TYPE_STRING);
			RO_PROP_FN(getNumWires,	 get_num_wires,	 TYPE_INT);
			RO_PROP_FN(isMaster,	 is_master,		 TYPE_bool);
			RO_PROP_FN(isSlave,		 is_slave,		 TYPE_bool);
			RO_PROP_FN(isTwoWay,	 is_two_way,	 TYPE_bool);
			PROP_FNS(getSlaveAnimation, get_slave_animation, setSlaveAnimation, set_slave_animation, TYPE_CONTROL);
		END_FUNCTION_MAP

};


#endif // NO_PARAMETER_WIRING

#endif
