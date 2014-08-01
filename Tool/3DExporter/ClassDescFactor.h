#ifndef _CCLASSDESCFACTOR_H_
#define _CCLASSDESCFACTOR_H_
#include "export.h"
#include "ipoint2.h"
class INode;
class ReferenceMaker;
#include "coreexp.h"
#include "tab.h"
#include "strbasic.h"
#include "box2.h" // for Rect. Cannot forward declare this.
#include "plugapi.h"

class ClassDescFactor : public ClassDesc
{
public:
	ClassDescFactor();
	~ClassDescFactor();

	virtual int				IsPublic();
	virtual void*			Create(BOOL loading = FALSE);
	virtual const MCHAR*	ClassName();
	virtual SClass_ID		SuperClassID();
	virtual Class_ID		ClassID();
	virtual const MCHAR*	Category();
	virtual MCHAR* GetRsrcString(INT_PTR id);
};

#endif
