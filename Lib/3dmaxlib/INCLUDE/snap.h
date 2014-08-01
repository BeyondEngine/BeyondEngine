/**********************************************************************
 *<
	FILE: snap.h

	DESCRIPTION: Definitions for snap functionality

	CREATED BY: Tom Hudson

	HISTORY:

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _SNAP_H_

#define _SNAP_H_

#include "maxheap.h"

// Snap types 
#define SNAP_2D		1		// 2-D Snap
#define SNAP_25D	2		// 2 1/2-D Snap
#define SNAP_3D		3		// 3-D Snap

// Snap modes

#define SNAPMODE_RELATIVE	0
#define SNAPMODE_ABSOLUTE	1

// Snap flags

#define SNAP_IN_3D								(0)			// Snap to all points (looks dumb here, but code reads easier)
#define SNAP_IN_PLANE							(1<<0)	// Snap only to points in plane
#define SNAP_UNSEL_OBJS_ONLY			(1<<1)	// Ignore selected nodes
#define SNAP_SEL_OBJS_ONLY				(1<<2)	// Ignore unselected nodes
#define SNAP_UNSEL_SUBOBJ_ONLY		(1<<3)	// Ignore selected geometry
#define SNAP_SEL_SUBOBJ_ONLY			(1<<4)	// Ignore unselected geometry
#define SNAP_FORCE_3D_RESULT			(1<<5)	// Override user settings to force snap in 3D
#define SNAP_OFF_PLANE						(1<<6)  // snap Only to points off the plane
#define SNAP_TRANSPARENTLY				(1<<7)  // This suppresses any display in the views

#define SNAP_APPLY_CONSTRAINTS		(1<<8)  //
#define SNAP_PROJ_XAXIS						(1<<9)	//
#define SNAP_PROJ_YAXIS						(1<<10)	//
#define SNAP_PROJ_ZAXIS						(1<<11)	//

#define SNAP_XFORM_AXIS						(1<<12)	// Informs the osnapmanager to invalidate the com axis
#define SNAP_BEGIN_SEQ						(1<<13)	//
#define SNAP_END_SEQ							(1<<14)	//
#define SNAP_USE_XFORM_AXIS				(1<<15)	// If set, the origin of the selection's transform matrix
																					// will be used as snap point, in case no hit point has been recorded

// Snap information	structure
/*! \sa <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_snap_flags.html">List of Snap Flags</a>, Class ViewExp.
\remarks This structure describes the snap settings used for an operation.
*/
struct SnapInfo: public MaxHeapOperators {
	// The snap settings for this operation
	/*! One of the following values:
	- SNAP_2D \n
	2-D Snap.
	- SNAP_25D \n
	2 1/2-D Snap.
	- SNAP_3D \n
	3-D Snap.
	*/
	int snapType;			// See above
	/*! Maximum snap distance. */
	int strength;			// Maximum snap distance
	/*! Geometry vertex priority. */
	int vertPriority;		// Geometry vertex priority
	/*! Geometry edge priority. */
	int edgePriority;		// Geometry edge priority
	/*! Grid intersection priority. */
	int gIntPriority;		// Grid intersection priority
	/*! Grid line priority. */
	int gLinePriority;		// Grid line priority
	/*! See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_snap_flags.html">List of Snap Flags</a>. */
	DWORD flags;			// See above
	/*! Plane to use for snap computations. */
	Matrix3 plane;			// Plane to use for snap computations
	// The best snap so far...
	/*! Best snap point in world space. */
	Point3 bestWorld;		// Best snap point in world space
	/*! Best snap point in screen space. */
	Point2 bestScreen;		// Best snap point in screen space
	/*! Best snap point distance. */
	int bestDist;			// Best snap point distance
	/*! Best point's priority. */
	int priority;			// Best point's priority
	};

#endif // _SNAP_H_
