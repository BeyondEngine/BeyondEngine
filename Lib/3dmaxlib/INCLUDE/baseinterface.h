/**********************************************************************
 *<
	FILE: baseInterface.h

	DESCRIPTION: Base classes for various interfaces in MAX

	CREATED BY: John Wainwright

	HISTORY: created 9/5/00

 *>	Copyright (c) 2000 Autodesk, All Rights Reserved.
 **********************************************************************/

#ifndef __BASEINTERFACE_H__
#define __BASEINTERFACE_H__

#include "maxheap.h"
#include "buildver.h"	// russom 02/26/01

class InterfaceServer;
class BaseInterfaceServer;
class BaseInterface;
class InterfaceNotifyCallback;

// ID for BaseInterfaces
#define BASEINTERFACE_ID Interface_ID(0, 1)

// Base class for those classes and interfaces in MAX that can serve interfaces.  
/*! \sa  Class BaseInterface, Class IObject, <a href="ms-its:3dsmaxsdk.chm::/function_root.html">Function Publishing System</a>.\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
Class InterfaceServer is the base class for interface servers in 3ds Max and
should be inherited by any class that wishes to implement the GetInterface()
protocol. The InterfaceServer also adds a data member for storing interfaces,
typically extension interfaces added to maintain API binary compatibility. <br>
*/
class InterfaceServer: public MaxHeapOperators
{
public:
	SDK_RESERVED_METHOD(1); // russom 02/26/01
	/*! \remarks Returns a pointer to the interface whose ID is specified.
	\par Parameters:
	<b>Interface_ID id</b>\n\n
	The ID of the interface to return.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual BaseInterface* GetInterface(Interface_ID id) { return NULL; }
	SDK_RESERVED_METHOD(2); // russom 02/26/01
};

// The base class for interfaces in MAX R4.  
//   Provides basic identity, sub-interface access, lifetime management and 
//   cloning methods.   The base class for FPInterface in the FnPub system.
//  
/*! \sa  Class InterfaceServer, Class InterfaceNotifyCallback,  Class FPInterface, <a href="ms-its:3dsmaxsdk.chm::/function_root.html">Function Publishing System.</a>\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class represents the base class for interfaces in 3ds Max. The
BaseInterface class should be used as the base class for any new interface
class in 3ds Max and provides basic identity, memory management, and cloning
methods. <b>Class FPInterface</b>, which is part of the Function Publishing
system, has the BaseInterface class as its base class.  */
class BaseInterface : public InterfaceServer
{
public:
	// from InterfaceServer
	BaseInterface* GetInterface(Interface_ID id) { if (id == BASEINTERFACE_ID) return this; else return NULL; }

	// identification
	/*! \remarks This method returns the unique interface ID. */
	virtual Interface_ID	GetID() { return BASEINTERFACE_ID; }

	// interface lifetime management
	//   there is an implied Acquire() whenever an interface is served via a GetInterface()
	//   nortmally requiring a matching Release() from the client
	//   can optionally use LifetimeControl() method to enquire about actual
	//   lifetime policy of client and provide a server-controlled delete notify callback
	//		'noRelease' means don't need to call release, use interface as long as you like. 
	//		'immediateRelease' means the interface is good for only one call, but the release 
	//		    is implied, you don't need to call release.
	//		'wantsRelease' means the clients are controlling lifetime so the interface needs 
	//			a Release() when the client has finished (default).
	//		'serverControlled' means the server controls lifetime and will use the InterfaceNotifyCallback 
	//			to tell you when it is gone.
	enum LifetimeType { noRelease, immediateRelease, wantsRelease, serverControlled }; 

	// LifetimeControl returns noRelease since 
	// AcquireInterface and ReleaseInterface do not perform 
	// any real acquiring and releasing (reference counting, etc.)
	// If the implementation of AcquireInterface and ReleaseInterface changes
	// in this class or derived classes, the return value of LifetimeControl 
	// needs to be updated accordingly.
	// RegisterNotifyCallback returns true if the callback will be called at or before deletion
	/*! \remarks This method allows enquiries into the actual lifetime policy
	of a client and provide a server-controlled delete notify callback.
	\return  One of the following LifetimeTypes:\n\n
	<b>noRelease</b>\n\n
	Do not call release, use interface as long as you like.\n\n
	<b>immediateRelease</b>\n\n
	The interface is only good for one calls. The release is implied so a call
	to release is not required.\n\n
	<b>wantsRelease</b>\n\n
	The clients are controlling the lifetime, so the interface needs a
	Release() when the client has finished. This is the default.\n\n
	<b>serverControlled</b>\n\n
	The server controls the lifetime and will use the InterfaceNotifyCallback
	to inform the code when it is gone.
	\par Default Implementation:
	<b>{ return noRelease; }</b> */
	virtual LifetimeType	LifetimeControl() { return noRelease; }

	/*! \remarks This method allows you to register an interface notify callback.
	\par Parameters:
	<b>InterfaceNotifyCallback* incb</b>\n\n
	A pointer to the interface notify callback.
	\par Default Implementation:
	<b>{ return false; }</b> */
	virtual bool			RegisterNotifyCallback(InterfaceNotifyCallback* incb) { return false; }
	/*! \remarks This method allows you to un-register an interface notify
	callback.
	\par Parameters:
	<b>InterfaceNotifyCallback incb</b>\n\n
	A pointer to the interface notify callback.
	\par Default Implementation:
	<b>{ }</b> */
	virtual void			UnRegisterNotifyCallback(InterfaceNotifyCallback* incb) { }
	/*! \remarks This method is part of the interface reference management and can
	be implemented by dynamically allocated interfaces for ref-count based lifetime
	control). This method should return TRUE if it needs <b>Release()</b> to be
	called.
	\par Default Implementation:
	<b>{ return false; }</b> */
	virtual BaseInterface*	AcquireInterface() { return (BaseInterface*)this; }; 
	/*! \remarks This method is called when a reference to this object is deleted.
	\par Default Implementation:
	<b>{ }</b> */
	virtual void			ReleaseInterface() { };

	// direct interface delete request
	/*! \remarks This method can be used as a direct interface delete request.
	\par Default Implementation:
	<b>{ }</b> */
	virtual void			DeleteInterface() { };

	// interface cloning
	/*! \remarks This method allows you to clone the base interface.
	\par Parameters:
	<b>void* remapDir</b>\n\n
	The RemapDir passed to the clone method.
	\par Default Implementation:
	<b>{ return NULL; }</b> */
	virtual BaseInterface*	CloneInterface(void* remapDir = NULL) { return NULL; }
};

// BaseInterface server specializes InterfaceServer with an implementation
//   based on a Tab<> of interface pointers for storing interfaces, 
//   typically extension interfaces, and providing an interface iteration
//   protocol.
//   class IObject in the FnPub system specializes BaseInterfaceServer 
/*! \sa  Class InterfaceServer, Class BaseInterface\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
The BaseInterface server class specializes the InterfaceServer class with an
implementation based on a Tab\<\> of interface pointers for storing interfaces,
typically extension interfaces, and providing an interface iteration protocol.
<b>class IObject</b> in the Function Publishing System specializes class
BaseInterfaceServer. The class contains a protected table of BaseInterface
pointers. <b>Class IObject</b> is an example of a class which is based on the
BaseInterfaceServer class.  */
class BaseInterfaceServer : public InterfaceServer
{
protected:
	Tab<BaseInterface*> interfaces;

public:
	// interface serving, default implementation looks in interfaces table
	/*! \remarks This method returns a pointer to the BaseInterface of the
	specified interface.
	\par Parameters:
	<b>Interface_ID id</b>\n\n
	The interface ID for which to return the BaseInterface. */
	virtual BaseInterface* GetInterface(Interface_ID id) 
	{ 
		for (int i = 0; i < interfaces.Count(); i++)
			if (interfaces[i]->GetID() == id)
				return interfaces[i]->AcquireInterface();
		return NULL; 
	}
	// interface enumeration...
	/*! \remarks This method returns the number of interfaces.
	\par Default Implementation:
	<b>{ return interfaces.Count(); }</b> */
	virtual int NumInterfaces() const { return interfaces.Count(); }					
	/*! \remarks This method returns a pointer to the BaseInterface of the
	I-th interface.
	\par Parameters:
	<b>int i</b>\n\n
	The index of the interface in the table.
	\par Default Implementation:
	<b>{ return interfaces[i]; }</b>\n\n
	  */
	virtual BaseInterface* GetInterfaceAt(int i) const { return interfaces[i]; }

	// default descructor, calls DeleteInterface() on all live entries in interfaces table
	/*! \remarks Destructor. */
	~BaseInterfaceServer()
	{
		for (int i = 0; i < interfaces.Count(); i++)
			if (interfaces[i] != NULL)
				interfaces[i]->DeleteInterface();
	}
};

// InterfaceNotifyCallback base class,  //   can be specialized by clients of an interface that controls its own lifetime
//   so that they can be notified when the interface is deleted or other changes occur.
//   registered with the interface via the lifetime control protocol 
/*! \sa  Class BaseInterface, <a href="ms-its:3dsmaxsdk.chm::/function_root.html">Function Publishing System.</a>\n\n
\par Description:
This class is available in release 4.0 and later only.\n\n
This class provides a callback mechanism which can be registered with an
interface on Acquire() so that it can be notified when the interface goes away
as the server controls the lifetime.  */
class InterfaceNotifyCallback: public MaxHeapOperators
{
public:
	// notify server is deleting  the interface
	/*! \remarks This method gets called to notify the server is deleting the
	interface.
	\par Parameters:
	<b>BaseInterface* bi</b>\n\n
	A pointer to the appropriate BaseInterface.
	\par Default Implementation:
	<b>{ }</b> */
	virtual void InterfaceDeleted(BaseInterface* bi) { }

	// for furture notification extensions
	/*! \remarks Returns a pointer to the interface whose ID is specified, for
	future notification extensions.
	\par Parameters:
	<b>Interface_ID id</b>\n\n
	The ID of the interface to return.
	\return  <b>{ return NULL; }</b> */
	virtual BaseInterface* GetInterface(Interface_ID id) { return NULL; }
};

#endif
