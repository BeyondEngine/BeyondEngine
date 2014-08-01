/**********************************************************************
 *<
	FILE: svcore.h

	DESCRIPTION: Schematic View Interface

	HISTORY: Extended by Michael Russo for R6

 *>	Copyright (c) 1994-2003, All Rights Reserved.
 **********************************************************************/


#ifndef __SVCORE_H__
#define __SVCORE_H__
#include "maxheap.h"

typedef enum
	{
	SVT_PROCEED,
	SVT_EXISTS,
	SVT_DO_NOT_PROCEED,
	} SvTraverseStatus;

typedef enum
	{
	REFTYPE_CHILD,
	REFTYPE_SUBANIM,
	REFTYPE_PLUGIN,
	} SvReferenceType;

typedef enum
	{
	RELTYPE_INSTANCE,
	RELTYPE_PARAMWIRE,
	RELTYPE_CONSTRAINT,
	RELTYPE_LIGHT,
	RELTYPE_MODIFIER,
	RELTYPE_CONTROLLER,
	RELTYPE_OTHER,
	} SvRelationshipType;

// Filter bits...
static const DWORD SV_FILTER_SELOBJECTS     = (1<<0);
static const DWORD SV_FILTER_OBJECTMODS     = (1<<1);
static const DWORD SV_FILTER_BASEPARAMS     = (1<<2);
static const DWORD SV_FILTER_MATPARAMS      = (1<<3);
static const DWORD SV_FILTER_GEOM           = (1<<4);
static const DWORD SV_FILTER_SHAPES         = (1<<5);
static const DWORD SV_FILTER_LIGHTS         = (1<<6);
static const DWORD SV_FILTER_CAMERAS        = (1<<7);
static const DWORD SV_FILTER_HELPERS        = (1<<8);
static const DWORD SV_FILTER_WARPS          = (1<<9);
static const DWORD SV_FILTER_VISIBLE_OBJS   = (1<<10);
static const DWORD SV_FILTER_CONTROLLERS    = (1<<11);
static const DWORD SV_FILTER_ANIMATEDONLY   = (1<<12);
static const DWORD SV_FILTER_MAPS           = (1<<13);	// obsolete
static const DWORD SV_FILTER_BONES          = (1<<14);	// obsolete
static const DWORD SV_FILTER_BONEOBJECTS    = (1<<15);
static const DWORD SV_FILTER_PB_PARAMS      = (1<<16);
static const DWORD SV_FILTER_PRS_POS        = (1<<17);
static const DWORD SV_FILTER_PRS_ROT        = (1<<18);
static const DWORD SV_FILTER_PRS_SCALE      = (1<<19);
static const DWORD SV_FILTER_MASTER_POINT   = (1<<20);
static const DWORD SV_FILTER_SKIN_DETAILS   = (1<<21);

// Schematic view UI colors...
static const int SV_UICLR_WINBK               = 0;
static const int SV_UICLR_NODEBK              = 1;
static const int SV_UICLR_SELNODEBK           = 2;
static const int SV_UICLR_NODE_HIGHLIGHT      = 3;
static const int SV_UICLR_MATERIAL_HIGHLIGHT  = 4;
static const int SV_UICLR_MODIFIER_HIGHLIGHT  = 5;
static const int SV_UICLR_PLUGIN_HIGHLIGHT    = 6;
static const int SV_UICLR_SUBANIM_LINE        = 7;
static const int SV_UICLR_CHILD_LINE          = 8;
static const int SV_UICLR_FRAME               = 9;
static const int SV_UICLR_SELTEXT             = 10;
static const int SV_UICLR_TEXT                = 11;
static const int SV_UICLR_FOCUS               = 12;
static const int SV_UICLR_MARQUIS             = 13;
static const int SV_UICLR_COLLAPSEARROW       = 14;
static const int SV_UICLR_GEOMOBJECT_BK       = 15;
static const int SV_UICLR_LIGHT_BK            = 16;
static const int SV_UICLR_CAMERA_BK           = 17;
static const int SV_UICLR_SHAPE_BK            = 18;
static const int SV_UICLR_HELPER_BK           = 19;
static const int SV_UICLR_SYSTEM_BK           = 20;
static const int SV_UICLR_CONTROLLER_BK       = 21;
static const int SV_UICLR_MODIFIER_BK         = 22;
static const int SV_UICLR_MATERIAL_BK         = 23;
static const int SV_UICLR_MAP_BK              = 24;
static const int SV_UICLR_GRID                = 25;
static const int SV_UICLR_REL_INSTANCE        = 26;
static const int SV_UICLR_REL_CONSTRAINT      = 27;
static const int SV_UICLR_REL_PARAMWIRE       = 28;
static const int SV_UICLR_REL_LIGHT           = 29;
static const int SV_UICLR_REL_MODIFIER        = 30;
static const int SV_UICLR_REL_CONTROLLER      = 31;
static const int SV_UICLR_REL_OTHER           = 32;
static const int SV_UICLR_SPACEWARP_BK        = 33;
static const int SV_UICLR_BASEOBJECT_BK       = 34;

static const int SV_UICLR_CACHE_SIZE          = 35;

// Magic value returned from Animatable::SvGetSwatchColor(...)
// to indicate that no swatch is to be drawn...
static const int SV_NO_SWATCH							= 0xFFFFFFFF;

//-------------------------------------------------------------------------
// Bit flags which can be passed to IGraphObjectManager::AddAnimatable(...)
// and Animatable::SvTraverseAnimGraph(....)
//-------------------------------------------------------------------------

// If set, newly created node will be in the hidden state.  If the node
// already exists in the graph, the flag is ignored...
static const DWORD SV_INITIALLY_HIDDEN          = 0x00000001;

// If set, shared instances of an animatable will produce multiple
// graph nodes in the schematic view instead of a single shared graph node...
static const DWORD SV_DUPLICATE_INSTANCES       = 0x00000002;

// If set, the newly created children of the newly created node
// will be in the hidden state.  If the node already exists in the graph,
// the flag is ignored.  Children of this node that already exist in
// the graph will not have their visibility state changed...
static const DWORD SV_INITIALLY_CLOSED          = 0x00000004;

//-------------------------------------------------------------------------
// Flags which can be passed to IGraphObjectManager::PushLevel(...)
//-------------------------------------------------------------------------

// This id, when passed to PushLevel(), indicates that no id is to be associated
// with Animatable being pushed onto the stack...
static const int SV_NO_ID                       = 0x80000000;

class IGraphNode;
class Animatable;
class IGraphObjectManager;

/*! \sa  Class Animatable.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This is the callback object used to perform the (de)selection via
<b>Animatable:: SvGetMultiSelectCallback()</b>.\n\n
Schematic view supports multiple selection. When the user selects a set of
objects in the schematic view and then "transfers" that selection set to the
rest of max (either by having "synchronize selection" on or by manually moving
the selection out), there are a number of ambiguities that can arise. For
example, some of the objects in the schematic view cannot be selected in the
viewports, material editor, or modifier stack. Another example: the material
editor only supports one active material/map but many materials and maps can be
selected simultaneously in the schematic view. The "MultiSelectCallback" system
exists order to handle these cases and to handle selection synchronization
between SV and future editors in 3ds Max. When the schematic view attempts to
synchronize selection by moving the SV selection set to the "outside" world, it
follows this procedure:\n\n
1. First SV calls SvGetMultiSelectCallback(...) on all the visible SV nodes to
"collect" MultiSelectCallback objects. Objects that want to synchronize their
selection state with the schematic view (not a common or trivial operation --
this is really more associated with adding a new editor in 3ds Max rather than
adding new plugin) return a pointer to a static instance of a
MultiSelectCallback derived object. There is only one instance of a
MultiSelectCallback per editor. Furthermore, if an editor displays objects of
many classes, all the classes should override SvGetMultiSelectCallback(...) to
return the same MultiSelectCallback instance. This implies that, as far as the
schematic view is concerned, there is never more than one primary editor class
associated with any particular object class (currently, viewports for nodes,
material editor for materials and maps and the modifier panel for
modifiers).\n\n
For example, here is the code in BaseNode that returns the MultiSelectCallback
instance for nodes (this is the MultiSelectCallback used for viewports):
\code
class BaseNodeMSelCB : public MultiSelectCallback
{
	private:
		bool clear;
		BaseNodeTab selNodeTab;
		BaseNodeTab deselNodeTab;

		int Priority() { return 1000; }
		void Begin(IGraphObjectManager *gom, bool clear);
		void Select(IGraphObjectManager *gom, IGraphNode *gNode, bool isSelected);
		void End(IGraphObjectManager *gom);
};
static BaseNodeMSelCB baseNodeMSelCB;
MultiSelectCallback* BaseNode::SvGetMultiSelectCallback(IGraphObjectManager *gom, IGraphNode *gNode)
{
	return &baseNodeMSelCB;
}
\endcode 
2. For each selection class (unique MultiSelectCallback instance), the
schematic views calls "Begin(...)". This is the spot where any "pre-selection"
preparation takes place. The order that the MultiSelectCallback instances are
called in is determined by their priority. The priority is returned by the
"Priority()" method. MultiSelectCallback's with a higher priority (lower value)
are called before those with a lower priority (higher value). For example, here
is the Begin associated with the viewports:
\code
void BaseNodeMSelCB::Begin(IGraphObjectManager *gom, bool clear)
{
	this->clear = clear;
//
// If the "clear" bool is true, the current viewport selection set is cleared...
//
		if (clear)
		GetActiveSelSet()->Clear(FALSE);
//
// Some housekeeping in preparation for the select...
//
	selNodeTab.Resize(0);
	deselNodeTab.Resize(0);
}
\endcode 
3. For each of objects in the schematic view whose selection state is changing,
the object's MultiSelectCallback instance is retrieved (again) and the "Select"
method is called. Here is where the actual selection/deselection work can take
place. I say "can" because, in practice, the select method usually just
collects all the objects to be selected and all the objects to be deselected
into lists which are then processed in the "End(...)" method. This is simply
for performance -- it is often more efficient to set the selection state of a
group of objects all at once. Here's the "Select(...)" method from
BaseNode:
\code
void Select(IGraphObjectManager *gom, IGraphNode *gNode, bool isSelected)
{
	BaseNode *baseNode = (BaseNode *) gNode->GetAnim();

	if (isSelected) {
		if (!baseNode->IsRootNode() && !baseNode->IsFrozen()
			&& !baseNode->IsHidden())
			selNodeTab.AppendNode(baseNode, FALSE);
	}
	else {
		if (baseNode->Selected())
			deselNodeTab.AppendNode(baseNode, FALSE);
	}
}
\endcode 
4. Finally, for each selection class (unique MultiSelectCallback instance), the
schematic views calls "End(...)". This is the spot where any "post-selection"
operations take place. For example, here is the "End(...)" for the BaseNode
(viewports):\n\n
\code
void End(IGraphObjectManager *gom)
{
	if (selNodeTab.Count() > 0 || deselNodeTab.Count() > 0) {
		theHold.Begin();
		if (selNodeTab.Count() > 0)
			GetActiveSelSet()->SelectMultiple(selNodeTab, FALSE);

		if (deselNodeTab.Count() > 0)
			GetActiveSelSet()->DeselectMultiple(deselNodeTab, FALSE);

		theHold.Accept(getResMgr().getString(IDS_SV_SELECT, appInst));
		RedrawViewports(GetCurTime(), VP_DONT_SIMPLIFY);
	}
	else {
		if (clear)
			RedrawViewports(GetCurTime(), VP_DONT_SIMPLIFY);
	}
}
\endcode  */
class MultiSelectCallback: public MaxHeapOperators
	{
	public:
	/*! \remarks Returns the priority of the callback. MultiSelectCallback's
	with a higher priority (lower value) are called before those with a lower
	priority (higher value). */
	virtual int Priority() = 0;		// Used for sorting select order.
	/*! \remarks Called to begin the multi-selection process. This is the spot
	where any "pre-selection" operations take place.
	\par Parameters:
	<b>IGraphObjectManager *gom</b>\n\n
	Points to the schematic view window manager.\n\n
	<b>bool clear</b>\n\n
	true to clear the previous selection; false to leave intact. */
	virtual void Begin(IGraphObjectManager *gom, bool clear) = 0;
	/*! \remarks This method selects or deselects the node passed.
	\par Parameters:
	<b>IGraphObjectManager *gom</b>\n\n
	Points to the schematic view window manager.\n\n
	<b>IGraphNode *gNode</b>\n\n
	Points to the node in schematic view.\n\n
	<b>bool isSelected</b>\n\n
	true if select; false if deselect. */
	virtual void Select(IGraphObjectManager *gom, IGraphNode *gNode, bool isSelected) = 0;
	/*! \remarks Called when done. This is the spot where any "post-selection"
	operations take place.
	\par Parameters:
	<b>IGraphObjectManager *gom</b>\n\n
	Points to the schematic view window manager. */
	virtual void End(IGraphObjectManager *gom) = 0;
	};

/*! Class IGraphObjectManager, Class IGraphNode, Class Object.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This class represents a node pointer and, currently, has no methods. */
class IGraphRef: public MaxHeapOperators
	{
	public:
	};

/*! \sa  Class IGraphObjectManager,  Class Object, Class Animatable.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This class represents a node in the schematic view graph and provides a few
methods for querying information about the node.  */
class IGraphNode: public MaxHeapOperators
	{
	public:

		// Returns the Animatable associated with this node...
	/*! \remarks Returns the Animatable associated with this node. */
	virtual Animatable *GetAnim() = 0;

		// Returns the "primary parent" of this node.  Nodes
		// can have multiple parents (objects referencing
		// this node) so this function is not strictly
		// accurate.  That said, many nodes have the
		// concept of an owner node, which is what this
		// function returns.
	/*! \remarks Returns the "primary parent" of this node. Nodes can have
	multiple parents (objects referencing this node) so this function is not
	strictly accurate. That said, many nodes have the concept of an owner node,
	which is what this function returns. */
	virtual IGraphNode *GetParentNode() = 0;

		// Returns the "owner" of this node.  Some nodes
		// have multiple owners.  When this is the case, this
		// function returns the "first" owner (the object
		// that first added this node to the schematic view)...
	/*! \remarks Returns the "owner" of this node. Some nodes have multiple
	owners. When this is the case, this function returns the "first" owner (the
	object that first added this node to the schematic view). */
	virtual Animatable *GetOwner() = 0;

		// Return the "id" of this node.  When nodes are
		// added to the schematic view (via the
		// IGraphObjectManager::AddAnimatable(...) method),
		// an integer is provided.  This value is is not
		// used internally by the schematic view.  Rather,
		// it is available to implementers of the 
		// Animatable::Sv*() methods to aid in identifying
		// the node.
	/*! \remarks Return the ID of this node. When nodes are added to the
	schematic view (via the <b>IGraphObjectManager::AddAnimatable(...)</b>
	method), an integer is provided. This value is not used internally by the
	schematic view. Rather, it is available to implementers of the
	<b>Animatable::Sv*()</b> methods to aid in identifying the node. */
	virtual int GetID() = 0;
	};

/*! \sa  Class IGraphNode, Class IGraphObjectManager, Class Animatable.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This is a small container type class that associates graph nodes with traversal
status.
\par Data Members:
All data members are public.\n\n
<b>IGraphNode *gNode;</b>\n\n
Points to the interface for the node in the schematic view.\n\n
<b>SvTraverseStatus stat;</b>\n\n
The traveral status. One of the following values:\n\n
<b>SVT_PROCEED</b>\n\n
The traversal should proceed.\n\n
<b>SVT_EXISTS</b>\n\n
This is not currently being used.\n\n
<b>SVT_DO_NOT_PROCEED</b>\n\n
The traversal should not proceed.  */
class SvGraphNodeReference: public MaxHeapOperators
	{
	public:

	IGraphNode *gNode;
	SvTraverseStatus stat;

	/*! \remarks Constructor. The data members are initialized as follows:\n\n
	<b>gNode= NULL;</b>\n\n
	<b>stat = SVT_DO_NOT_PROCEED;</b> */
	SvGraphNodeReference()
		{
		gNode= NULL;
		stat = SVT_DO_NOT_PROCEED;
		}
	};

/*! \sa  Class IGraphNode, Class IGraphRef, Class Animatable.\n\n
\par Description:
This class is available in release 3.0 and later only.\n\n
This class essentially represents an instance of a schematic view window and
provides methods for adding nodes and node pointers, refreshing the schematic
view, accessing filter bits and updating and controlling the various editors
within 3ds Max in ways that are not surfaced in the general interface.  */
class IGraphObjectManager: public MaxHeapOperators
	{
	public:

		// During traversal of the Animatable graph via SvTraverseAnimGraph(...),
		// PushLevel() and PopLevel() should be called appropriately to
		// maintain an ownership stack.  This is required by the schematic view
		// when nodes are added to the graph with the "SV_DUPLICATE_INSTANCES"
		// flag set...
	/*! \remarks During traversal of the Animatable graph via
	<b>SvTraverseAnimGraph(...)</b>, this method (and <b>PopLevel()</b> below)
	should be called appropriately to maintain an ownership stack. This is
	required by the schematic view when nodes are added to the graph with the
	<b>SV_DUPLICATE_INSTANCES</b> flag set. Note: In 3ds Max 3.0,
	<b>SV_DUPLICATE_INSTANCES</b> is always on (i.e., the flag is ignored).
	Because of this, <b>PushLevel()</b> and <b>PopLevel()</b> should always be
	called in <b>SvTraverseAnimGraph(...)</b>.\n\n
	See the sample code in <b>Animatable::SvStdTraverseAnimGraph()</b> for an
	example of this.
	\par Parameters:
	<b>Animatable *anim</b>\n\n
	This is the Animatable that you are, in all likelihood, about to add to the
	graph via the <b>AddAnimatable()</b> call.\n\n
	<b>int id = SV_NO_ID</b>\n\n
	This is also the same "id" you'd pass into <b>AddAnimatable()</b>. The "id"
	is only required in cases where it's impossible for the schematic view to
	distinguish between two (or more) children in the tree that have the same
	Animatable but represent different sub-anims. For example, a box has both
	its width and height set to the same controller (instanced). In the
	schematic view, this is still shown in tree form so we need the ID to
	distinguish between the "width" and "height" children. */
	virtual void PushLevel(Animatable *anim, int id = SV_NO_ID) = 0;
	/*! \remarks Pops a level off the animatable ownership stack. See
	<b>PushLevel()</b> above for details. */
	virtual void PopLevel() = 0;

		// Adds an Animatable to the schematic view...
	/*! \remarks Adds an Animatable to the schematic view. Note that "owner"
	and "id" are actually arbitrary - they are used to provide context for this
	Animatable. This means that the plug-in developer can set them to any
	value. They are not used internally by the schematic view except in the
	"Sv*" methods (which the developer can override). So, when you add an
	Animatable to the schematic view, you would typically add the owner
	(parent) Animatable as well as the sub-anim id. This allows you to, for
	example, easily return the name of the object when queried by the
	SvGetName(...) call (whose default implementation is shown below):\n\n
	<b>TSTR Animatable::SvGetName(IGraphObjectManager *gom, IGraphNode *gNode,
	bool isBeingEdited)</b>\n\n
	<b> {</b>\n\n
	<b> Animatable *owner;</b>\n\n
	<b> int subNum;</b>\n\n
	<b> TSTR name; </b>\n\n
	<b> owner = gNode-\>GetOwner();</b>\n\n
	<b> subNum = gNode-\>GetID();</b>\n\n
	<b> name = owner-\>SubAnimName(subNum);</b>\n\n
	<b> </b>\n\n
	<b> return name;</b>\n\n
	<b> }</b>
	\par Parameters:
	<b>Animatable *anim</b>\n\n
	Points to the animatable to add.\n\n
	<b>Animatable *owner</b>\n\n
	Points to the owner of <b>anim</b> above (typically).\n\n
	<b>int id</b>\n\n
	When nodes are added to the schematic view via this method this integer is
	provided. This value is not used internally by the schematic view. Rather,
	it is available to implementers of the <b>Animatable::Sv*()</b> methods to
	aid in identifying the node.\n\n
	<b>DWORD flags = 0</b>\n\n
	This flag is some combination of the following bit flags. See
	<a href="ms-its:listsandfunctions.chm::/idx_R_list_of_sch_view_addanimflags.html">List of Schematic %View
	AddAnimatable Flags</a>.
	\return  A SvGraphNodeReference object. */
	virtual SvGraphNodeReference AddAnimatable(Animatable *anim, Animatable *owner, int id, DWORD flags = 0) = 0;

		// Add a reference from "maker" node to "target"...
	/*! \remarks This method adds a reference from the specified "maker" node
	to the specified "target" node.
	\par Parameters:
	<b>IGraphNode *maker</b>\n\n
	Points to the 'maker' node in schematic view.\n\n
	<b>IGraphNode *target</b>\n\n
	Points to the 'target' node in schematic view.\n\n
	<b>SvReferenceType type</b>\n\n
	One of the following enum values:\n\n
	<b> REFTYPE_CHILD</b>\n\n
	<b> REFTYPE_SUBANIM</b>\n\n
	<b> REFTYPE_PLUGIN</b>
	\return  A pointer to an IGraphRef object.
	\par Sample Code:
	\code
	SvGraphNodeReference Control::SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags)
	{
		int i;
		SvGraphNodeReference nodeRef;
		SvGraphNodeReference childNodeRef;
	
	//
	// Test filter to see if "Controllers" are active.
	// Bail out if they're off (being filtered out)...
	//
		if (!gom->TestFilter(SV_FILTER_CONTROLLERS))
			return SvGraphNodeReference();
	
	//
	// Push this level in the tree. Note that the sub-anim id is passed
	// in here because it's possible that the same instance of this control
	// may exist in multiple tracks of "owner".
	//
		gom->PushLevel(this, id);
	
	//
	// Some flags are set here pertaining to the control being added.
	// Note that the flags are also propagated down the tree
	// by passing them on to SubAnim(i)->SvTraverseAnimGraph(gom, this, i, flags);
	// SV_DUPLICATE_INSTANCES tells the schematic view not to
	// represent multiple instances with a single node. Instead they
	// are represented by multiple nodes in the schematic view
	// with the "triangle thingy" attached to the side to indicate
	// shared instances. This flag is ignored in R3 because
	// this mode of operation is globally enabled
	// SV_INITIALLY_HIDDEN tells the schematic view that this
	// control's node is to be initially displayed in the closed state.
	// Note that this has no effect if the node already exists
	// in the graph -- it only applies to newly added nodes.
	//
		flags |= SV_DUPLICATE_INSTANCES | SV_INITIALLY_HIDDEN;
	
	//
	// The control is added to the schematic view...
	//
		nodeRef = gom->AddAnimatable(this, owner, id, flags);
		if (nodeRef.stat == SVT_PROCEED) {
	//
	// This control's sub-anims are iterated over...
	//
			for (i = 0; i < NumSubs(); i++) {
				if (SubAnim(i)) {
	//
	// SvTraverseAnimGraph(...) is recursively called to add this sub-anim (and all its descendents) to the graph...
	//
					childNodeRef = SubAnim(i)->SvTraverseAnimGraph(gom, this, i, flags);
	
	//
	// Now a link (node pointer) is created in the schematic between the control (nodeRef.gNode) and its child sub-anim (childNodeRef.gNode)....
	//
					if (childNodeRef.stat != SVT_DO_NOT_PROCEED)
						gom->AddReference(nodeRef.gNode, childNodeRef.gNode, REFTYPE_SUBANIM);
				}
			}
		}
	
	//
	// The tree level is popped. Note: a PopLevel() call must always be	paired with a PushLevel() call!
	//
		gom->PopLevel();
	
		return nodeRef;
	}
	\endcode  */
	virtual IGraphRef *AddReference(IGraphNode *maker, IGraphNode *target, SvReferenceType type) = 0;

		// Add a relationship from "maker" node to another animatable
	virtual IGraphRef *AddRelationship( IGraphNode *maker, Animatable *anim, int id, SvRelationshipType type) = 0;

		// Pops up the property editor dialog on the
		// selected nodes in the schematic view...
	/*! \remarks Pops up the property editor dialog on the selected nodes in
	the schematic view. */
	virtual void SvEditSelectedNodeProperties() = 0;

		// Selects the given node in the material editor.
		// Does nothing if "gNode" does not represent a
		// material or map...
	/*! \remarks Selects the given node in the material editor. Does nothing
	if "gNode" does not represent a material or map.
	\par Parameters:
	<b>IGraphNode *gNode</b>\n\n
	Points to the node in schematic view. */
	virtual void SvSelectInMaterialEditor(IGraphNode *gNode) = 0;

		// Selects the given node in the modifier panel.
		// Does nothing if "gNode" does not represent an
		// object...
	/*! \remarks Selects the given node in the modifier panel. Does nothing if
	"gNode" does not represent an object.
	\par Parameters:
	<b>IGraphNode *gNode</b>\n\n
	Points to the node in schematic view. */
	virtual void SvSetCurEditObject(IGraphNode *gNode) = 0;

		// Returns true if the given node is current
		// in the modifier panel...
	virtual bool SvIsCurEditObject(IGraphNode *gNode) = 0;

	virtual bool ApplyModifier(IGraphNode *gModNode, IGraphNode *gParentNode) = 0;
	virtual bool DeleteModifier(IGraphNode *gNode) = 0;

	virtual bool ApplyController(IGraphNode *gSrcNode, IGraphNode *gDestNode) = 0;

		// Invalidates the schematic view window...
	/*! \remarks Invalidates the schematic view window. */
	virtual void SvInvalidateView() = 0;

		// Invalidates a node in the schematic view window...
	/*! \remarks Invalidates a node in the schematic view window.
	\par Parameters:
	<b>IGraphNode *gNode</b>\n\n
	Points to the node in schematic view. */
	virtual void SvInvalidateNode(IGraphNode *gNode) = 0;

		// Forces the material editor to update...
	/*! \remarks Forces the material editor to update. */
	virtual void SvUpdateMaterialEditor() = 0;

		// Forces the modifier panel to update...
	/*! \remarks Forces the modifier panel to update. */
	virtual void SvUpdateModifierPanel() = 0;

		// Set, Clear and Test filter flags...
	/*! \remarks Sets the specified filter bits.
	\par Parameters:
	<b>DWORD mask</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_igraphobjectman_filter_bits.html">List of
	IGraphObjectManager Filter Bits</a>. */
	virtual void SetFilter(DWORD mask) = 0;
	/*! \remarks Clears the specified filter bits.
	\par Parameters:
	<b>DWORD mask</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_igraphobjectman_filter_bits.html">List of
	IGraphObjectManager Filter Bits</a>. */
	virtual void ClearFilter(DWORD mask) = 0;
	/*! \remarks Tets the specified filter bits. Returns true if set;
	otherwise false.
	\par Parameters:
	<b>DWORD mask</b>\n\n
	See <a href="ms-its:listsandfunctions.chm::/idx_R_list_of_igraphobjectman_filter_bits.html">List of
	IGraphObjectManager Filter Bits</a>. */
	virtual bool TestFilter(DWORD mask) = 0;

		// Get a SV UI color given a color index...
	virtual COLORREF SvGetUIColor(int colorIndex) = 0;

		// Get HWnd for Schematic View window...
	virtual HWND GetHWnd() = 0;
	};

#endif	// __SVCORE_H__

