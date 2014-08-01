
/*******************************************************************
 *
 *    DESCRIPTION:      Basic string definitions header file
 *
 *    AUTHOR:           Tom Hudson
 *
 *    HISTORY:          File created 9/6/94
 *
 *******************************************************************/

#ifndef _STRBASICS_
#define _STRBASICS_

#define WIN95STUFF

// To set up Max to use Unicode, define _UNICODE, and don't define _MBCS
// To set up Max to use multi-byte character sets, define _MBCS and 
//              don't define _UNICODE
// To set up Max to use single-byte ANSI character strings, don't define
//              either _UNICODE or _MBCS

// #define _UNICODE     // turn on Unicode support

#ifndef _MBCS
#define _MBCS   // if Unicode is off, turn on multi-byte character support
#endif


#ifdef _UNICODE

#ifdef _MBCS
#undef _MBCS    // can't have both Unicode and MBCS at once -- Unicode wins
#endif

#undef UNICODE
#define UNICODE

#undef STRCONST
#define STRCONST L

#endif

// Bring in the generic international text header file
#include <tchar.h>

// Starting with VS2005 the default is to have wchar_t defined as a type, so that's
// what we'll use as well (plus it coincides with the VS2005 Max9 release)
typedef __wchar_t       mwchar_t;

//
// MAX is not compiled in UNICODE (yet).  TCHAR has been changed to MCHAR where
// appropriate, so you can compile in UNICODE or not, and not have Max's headers
// change.
// 

#define MCHAR           char
#define LPCMSTR         const MCHAR*
#define _M(s)           (s)

// Similarly, rather than use _T("") when passing a string to Max, use _M("")
#define _M(p)           (p)

// These macros work in conjunction with the VC conversion macros (A2W, A2T, W2T, etc)
// See the MSDN documentation for details.  If you get compile errors such as
// 
// error C2065: '_lpa' : undeclared identifier
// error C2065: '_convert' : undeclared identifier
// error C2065: '_acp' : undeclared identifier
// 
// when trying to use any of these macros, it's probably because there's a
// 
// USES_CONVERSION
// 
// missing in your function.
#define M2A(p)          (p)
#define A2M(p)          (p)
#define M2W(p)          A2W(p)
#define W2M(p)          W2A(p)
#define M2T(p)          A2T(p)
#define T2M(p)          T2A(p)

#endif // _STRBASICS_
