/**********************************************************************
 *<
	FILE: IHardwareMNMesh.h

	DESCRIPTION: Hardware MNMesh Extension Interface

	CREATED BY: Norbert Jeske

	HISTORY: Created 10/11/00

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef _IHARDWARE_MNMESH_H_
#define _IHARDWARE_MNMESH_H_

#define IHARDWARE_MNMESH_INTERFACE_ID Interface_ID(0x65b154eb, 0x87d2b74)

class MNMesh;

class IHardwareMNMesh : public BaseInterface
{
public:
	virtual Interface_ID	GetID() { return IHARDWARE_MNMESH_INTERFACE_ID; }

	// Interface Lifetime
	virtual LifetimeType	LifetimeControl() { return noRelease; }
};

#endif
