/**********************************************************************
 *<
	FILE: strclass.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __STRCLASS__H
#define __STRCLASS__H
#include <windows.h>
#include "maxheap.h"
#include "strbasic.h"
#include "tab.h"

/*-------------------------------------------------------------------------------
  SR NOTE64

  "Only" supports 2G strings.

-------------------------------------------------------------------------------*/ 

//-----------------------------------------------------------------------
// CStr: Simple char string class
//-----------------------------------------------------------------------
/*! \sa  Class WStr, <a href="ms-its:3dsmaxsdk.chm::/ui_character_strings.html">Character Strings</a>.\n\n
\par Description:
A simple character string class. This is the standard character string class
used in 3ds Max. Methods and operators are provided for calculating lengths,
concatenation, substring operations, character searching, case conversion,
comparison, and formatted writing.\n\n
This class automatically allocates the proper amount of space for the string.
This is very handy in the case of internationalization / localization. For
example, if you code something like:\n\n
<b>TSTR myString = GetString(IDS_STRING_ID);</b>\n\n
then <b>myString</b>'s constructor will allocate enough space to store the
resource string no matter how long it is. This is much better than doing the
following:\n\n
<b>TCHAR myString[64];</b>\n\n
<b>_tcscpy(myString, GetString(IDS_STRING_ID));</b>\n\n
because the resource string may turn out to be much longer than 64 bytes once
it's translated to German or French (or whatever).\n\n
As another example, if you have the following code:\n\n
<b>TSTR str1 = _T("This is string1.");</b>\n\n
<b>TSTR str2 = _T("This is string2.");</b>\n\n
Then\n\n
<b>TSTR concatStr = str1 + str2;</b>\n\n
will again yield a (concatenated) string will enough space to hold the
concatenated contents of <b>str1</b> and <b>str2</b>, automatically.\n\n
All methods are implemented by the system. <br> 
\note The memory occupied by a CStr object is cannot be larger than 2Gb.
*/
class CStr: public MaxHeapOperators {
	char *buf;
	public:
		/*! \remarks Constructor. The string is set to NULL. */
		UtilExport CStr(); 
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport CStr(const char *cs);
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport CStr(const mwchar_t *wcstr);
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport CStr(const CStr& ws);
		/*! \remarks Destructor. The string is deleted. */
		UtilExport ~CStr(); 
		/*! \remarks Returns a pointer to the string. If the string is NULL, 0	is returned. */
		UtilExport char *data();
		/*! \remarks		Returns a pointer to the string. If the string is NULL, 0 is returned.*/
		UtilExport const char *data() const;
		/*! \remarks Returns a pointer to the string. If the string is NULL, 0	is returned. */
		UtilExport operator char *();

		// realloc to nchars (padding with blanks)
		/*! \remarks Reallocates the string to contain <b>nchars</b>
		characters. If the string is enlarged it is padded with blanks.
		\par Parameters:
		<b>int nchars</b>\n\n
		Specifies the new number of characters for the string. */
		UtilExport void Resize(int nchars);

		/*! \remarks Returns the number of characters in the string. */
		UtilExport int Length() const;
		/*! \remarks Returns the number of characters in the string. */
		int length() const { return Length(); }
		/*! \remarks Returns TRUE if the string length is 0; otherwise FALSE.
		*/
		BOOL isNull() const { return Length()==0?1:0; }

		/*! \remarks Assignment operator. */
		UtilExport CStr & operator=(const CStr& cs);
		/*! \remarks Assignment operator. */
		UtilExport CStr & operator=(const mwchar_t *wcstr);
		/*! \remarks Assignment operator. In release 3.0 and later this method	check for self-assignment. */
		UtilExport CStr & operator=(const char *cs);

		// Concatenation operators.
		/*! \remarks Concatenation operator. Returns a new string that is this
		string with string <b>cs</b> appended. */
		UtilExport CStr operator+(const CStr& cs) const;
		/*! \remarks Concatenation. Returns this string with <b>cs</b>	appended. */
		UtilExport CStr& operator+=(const CStr& cs); 
		/*! \remarks Concatenation. Returns this string with <b>cs</b>	appended. */
		CStr& Append(const CStr& cs)  { return ((*this) += cs); }
		/*! \remarks Concatenation. Returns this string with <b>cs</b>	appended to the end. */
		CStr& append(const CStr& cs)  { return ((*this) += cs); }
		/*! \remarks Returns this string with all characters from <b>pos</b>	to the end removed.
		\par Parameters:
		<b>int pos</b>\n\n
		Specifies the last position in the string. */
		UtilExport CStr& remove(int pos);	// remove all chars from pos to end
		/*! \remarks Returns this string with N characters removed from <b>pos</b> to the end.
		\par Parameters:
		<b>int pos</b>\n\n
		Specifies the position to begin removing characters.\n\n
		<b>int N</b>\n\n
		Specifies the number of characters to remove. */
		UtilExport CStr& remove(int pos, int N);	// remove N chars from pos to end

		// Substring operator
		/*! \remarks Returns a substring of this string, beginning at position
		<b>start</b>, of length <b>nchars</b>. */
		UtilExport CStr Substr(int start, int nchars) const;
		/*! \remarks Returns a substring of this string beginning at position	<b>i</b>. */
		UtilExport char& operator[](int i);
		// SR NOTE64: was const char&, which is slower but would also confuse 64 builds.
		/*! \remarks Returns a substring of this string beginning at position	<b>i</b>. */
		UtilExport char operator[](int i) const;	

		// Char search:(return -1 if not found)
		/*! \remarks Returns the index of the first occurrence of character
		<b>c</b> in this string. Returns -1 if not found. */
		UtilExport int first(char c) const;
		/*! \remarks Returns the index of the last occurrence of character
		<b>c</b> in this string. Returns -1 if not found. */
		UtilExport int last(char c) const;

		// Comparison
		/*! \remarks Equality operator.
		\return  Nonzero if the strings are equal; otherwise 0. */
		UtilExport int operator==(const CStr &cs) const;
		/*! \remarks Inequality operator.
		\return  Zero if the strings are equal; otherwise 1. */
		UtilExport int operator!=(const CStr &cs) const;
		/*! \remarks Returns nonzero if this string is less than <b>cs</b>; otherwise 0. */
		UtilExport int operator<(const CStr &cs) const;
		/*! \remarks Returns nonzero if this string is less than or equal to <b>ws</b>; otherwise 0. */
		UtilExport int operator<=(const CStr &ws) const;
		/*! \remarks Returns nonzero if this string is greater than <b>ws</b>;	otherwise 0. */
		UtilExport int operator>(const CStr &ws) const;
		/*! \remarks Returns nonzero if this string is greater than or equal	to <b>ws</b>; otherwise 0. */
		UtilExport int operator>=(const CStr &ws) const;

		/*! \remarks Converts all character of this string to uppercase. */
		UtilExport void toUpper();
		/*! \remarks Converts all character of this string to lowercase. */
		UtilExport void toLower();

		/*! \brief Write a formatted string into this CStr.
		 * Writes the format string, filled in by the optional arguments into
		 * this CStr.  See the ISO C++ documentation for more information on 
		 * printf and format strings.
		 * \param format Specifies how to format the destination string.
		 * \param ... optional arguments to format into the destination string.
		 * \pre format is not null.
		 * \pre There are the correct number of elliptical arguments to fill the 
		 * format string.
		 * \post This string is replaced with the formatted string.
		 * \return The number of characters written to this string, not including
		 * the null terminating character, or a negative value if an error occurs.
		 */
		UtilExport int printf(const char *format, ...);

		/*! \brief Write a formatted string into this CStr.
		This method is similar to CStr::printf. Instead of taking a variable list of 
		arguments as parameter, it takes a structure representing a variable list of 
		argument. This allows CStr objects to be used to build strings based on a 
		format string and a variable number of arguments:
		\code
		void LogMessage(CStr* format, ...) {
			va_list args;
			va_start(args, format);
			CStr buf;
			buf.printf(format, args);
			va_end(args);
			// log the message contained by buf
		}
		\endcode
		*/
		UtilExport int vprintf(const char *format, va_list args);
	};


//-----------------------------------------------------------------------
// WStr: Simple Wide char string class
//-----------------------------------------------------------------------
/*! \sa  Class CStr, <a href="ms-its:3dsmaxsdk.chm::/ui_character_strings.html">Character Strings</a>.\n\n
\par Description:
A wide character string class. This class uses 16 bits to hold each character.
Methods and operators are provided for calculating lengths, concatenation,
substring operations, character searching, case conversion, comparison, and
formatted writing. All methods are implemented by the system.\n\n
OLE file IO requires the wide characters of WStr. <br> 
\note The memory occupied by a WStr object is cannot be larger than 2Gb.
*/
class WStr: public MaxHeapOperators {
	mwchar_t *buf;
	public:
		/*! \remarks Constructor. The string is set to NULL. */
		UtilExport WStr();
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport WStr(const char *cs);
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport WStr(const mwchar_t *wcstr);
		/*! \remarks Constructor. The string is initialized to the string passed. */
		UtilExport WStr(const WStr& ws);
		/*! \remarks Destructor. The string is deleted. */
		UtilExport ~WStr();
		/*! \remarks Returns a pointer to the string. If the string is NULL, 0 is returned. */
		UtilExport mwchar_t *data();
		/*! \remarks		Returns a pointer to the string. If the string is NULL, 0 is returned. */
		UtilExport const mwchar_t *data() const;
		/*! \remarks Returns a pointer to the string. If the string is NULL, 0 is returned. */
		UtilExport operator mwchar_t *();

		// realloc to nchars (padding with blanks)
		/*! \remarks Reallocates the string to contain <b>nchars</b>	characters. 
		If the string is enlarged it is padded with blanks.
		\par Parameters: 
		<b>int nchars</b>\n\n
		Specifies the new number of characters for the string. */
		UtilExport void Resize(int nchars);
		/*! \remarks Returns the number of characters in the string. */
		UtilExport int Length() const;
		/*! \remarks Returns the number of characters in the string. */
		int length() const { return Length(); }
		/*! \remarks Returns TRUE if the string length is 0; otherwise FALSE.*/
		BOOL isNull() const { return Length()==0?1:0; }

		/*! \remarks Assignment operator. In release 3.0 and later this
		operator checks for self-assignment. */
		UtilExport WStr & operator=(const WStr& ws);
		/*! \remarks Assignment operator. */
		UtilExport WStr & operator=(const mwchar_t *wcstr);
		/*! \remarks Assignment operator. */
		UtilExport WStr & operator=(const char *cstr);

		// Concatenation operators.
		/*! \remarks Concatenation operator. Returns a new string that is this
		string with string <b>ws</b> appended. */
		UtilExport WStr operator+(const WStr& ws) const; 
		/*! \remarks Concatenation. Returns this string with <b>ws</b>	appended. */
		UtilExport WStr & operator+=(const WStr& ws); 
		/*! \remarks Concatenation. Returns this string with <b>ws</b> appended. */
		WStr& Append(const WStr& ws) { return ((*this) += ws); }
		/*! \remarks Concatenation. Returns this string with <b>ws</b> appended. */
		WStr& append(const WStr& ws)  { return ((*this) += ws); }
		/*! \remarks Returns this string with N characters removed from <b>pos</b> to the end.
		\par Parameters:
		<b>int pos</b>\n\n
		Specifies the position to begin removing characters.\n\n
		<b>int N</b>\n\n
		Specifies the number of characters to remove. */
		UtilExport WStr& remove(int pos);	// remove chars from pos to end
		UtilExport WStr& remove(int pos, int N);	// remove N chars from pos to end

		// Substring operator
		/*! \remarks Returns a substring of this string, beginning at position
		<b>start</b>, of length <b>nchars</b>. */
		UtilExport WStr Substr(int start, int nchars) const;
		/*! \remarks Returns a substring of this string beginning at position <b>i</b>. */
		mwchar_t& operator[](int i) {
			DbgAssert((buf != NULL) && (i >= 0) && (i <= Length())); 
			return buf[i];
		}
		/*! \remarks Returns a substring of this string beginning at position <b>i</b>. */
		mwchar_t operator[](int i) const {
			DbgAssert((buf != NULL) && (i >= 0) && (i <= Length())); 
			return buf[i];
		}

		// Char search:(return -1 if not found)
		/*! \remarks Returns the index of the first occurrence of character <b>c</b>
		in this string. Returns -1 if not found. */
		UtilExport int first(mwchar_t c) const;
		/*! \remarks Returns the index of the last occurrence of character <b>c</b> in
		this string. Returns -1 if not found. */
		UtilExport int last(mwchar_t c) const;

		// Comparison
		/*! \remarks Equality operator.
		\return  Nonzero if the strings are equal; otherwise 0. */
		UtilExport int operator==(const WStr &ws) const;
		/*! \remarks Inequality operator.
		\return  Zero if the strings are equal; otherwise 1. */
		UtilExport int operator!=(const WStr &ws) const;
		/*! \remarks Returns nonzero if this string is less than <b>ws</b>; otherwise 0. */
		UtilExport int operator<(const WStr &ws) const;
		/*! \remarks Returns nonzero if this string is less than or equal to	<b>ws</b>; otherwise 0. */
		UtilExport int operator<=(const WStr &ws) const;
		/*! \remarks Returns nonzero if this string is greater than <b>ws</b>;	otherwise 0. */
		UtilExport int operator>(const WStr &ws) const;
		/*! \remarks Returns nonzero if this string is greater than or equal	to <b>ws</b>; otherwise 0. */
		UtilExport int operator>=(const WStr &ws) const;

		/*! \remarks Converts all character of this string to uppercase. */
		UtilExport void toUpper();
		/*! \remarks Converts all character of this string to lowercase. */
		UtilExport void toLower();

		/**
		 * \brief Write a formatted string into this WStr.
		 *
		 * Writes the format string, filled in by the optional arguments into
		 * this WStr.  See the ISO C++ documentation for more information on 
		 * printf and format strings.
		 *
		 * \param format Specifies how to format the destination string.
		 * \param ... optional arguments to format into the destination string.
		 * \pre format is not null.
		 * \pre There are the correct number of elliptical arguments to fill the 
		 * format string.
		 * \post This string is replaced with the formatted string.
		 * \return The number of characters written to this string, not including
		 * the null terminating character, or a negative value if an error occurs.
		 */
		UtilExport int printf(const mwchar_t *format, ...);

		/*! \brief Write a formatted string into this WStr.
		This method is similar to WStr::printf. Instead of taking a variable list of 
		arguments as parameter, it takes a structure representing a variable list of 
		argument. This allows WStr objects to be used to build strings based on a 
		format string and a variable number of arguments:
		\code
		void LogMessage(WStr* format, ...) {
		va_list args;
		va_start(args, format);
		WStr buf;
		buf.printf(format, args);
		va_end(args);
		// log the message contained by buf
		}
		\endcode
		*/
		UtilExport int vprintf(const mwchar_t *format, va_list args);
	};					

#ifdef _UNICODE
#define TSTR WStr
#else
#define TSTR CStr
#endif

// MSTR will use the Max character type.
typedef CStr    MSTR;

//--FilterList----------------------------------------------------------------------
// A class whose sole purpose is for buildingup a  filter list to passing to
// GetSaveFileName and GetOpenFileName.  It automatically puts in the imbedded nulls
// and two terminating nulls.
//	 Example:
//
//	FilterList filterList;
//	filterList.Append( _T("Max files(*.max)"));
//	filterList.Append( _T("*.max"));
//	ofn.lpstrFilter  = filterList;
//	GetSaveFileName(&ofn)
//----------------------------------------------------------------------------------

/*! class FilterList
\par Description:
A class whose sole purpose is for building up a filter list to pass to the
Windows API functions <b>GetSaveFileName()</b> and <b>GetOpenFileName()</b>. It
automatically puts in the embedded nulls and two terminating nulls. All methods
are implemented by the system.\n\n
Example usage:\n\n
<b>FilterList filterList;</b>\n\n
<b>filterList.Append(_T("MAX files(*.max)"));</b>\n\n
<b>filterList.Append(_T("*.max"));</b>\n\n
<b>ofn.lpstrFilter = filterList;</b>\n\n
<b>GetSaveFileName(\&ofn)</b>
\par Data Members:
<b>#define LISTBUFLEN 2048</b>\n\n
<b>TCHAR buf[LISTBUFLEN];</b>\n\n
<b>int length;</b>  */
class FilterList: public MaxHeapOperators {
    Tab<MCHAR> buf;
	public:
		/*! \remarks Constructor. Sets <b>buf</b> to all zeros and sets the
		<b>length</b> to 0.
		\return  A new FilterList object. */
		UtilExport FilterList();
		/*! \remarks Appends the string passed to <b>buf</b>. */
		UtilExport void Append(MCHAR *name);
		UtilExport void Append(FilterList& filters);
		/*! \remarks Returns the address of <b>buf</b>. */
		UtilExport operator MCHAR *(); 
	};


/*! \defgroup SplitFilename SplitFilename - Split a filename into its components 
These function is used to extract the path, filename and/or extension out of a fully-qualified path name.

Pass in NULL for components which you do not require.

	\code
	{
		CStr p, f, e;
		CStr name("c:\\mydir\\myfile.txt");
	
		SplitFilename(name, &p, &f, &e);
	
		// p now contains "c:\\mydir"
		// f now contains "myfile"
		// e now contains ".txt"
	}

	{
		WStr p;
		WStr name(L"c:\\mydir\\myfile.txt");

		SplitFilename(name, &p, NULL, NULL);	

		// p now contains L"c:\\mydir"
	}

	{
		wchar_t file[MAX_PATH];

		SplitFilename(L"c:\\mydir\\myfile.txt", NULL, file, NULL);

		// file now contains L"myfile"
	}
	\endcode

	\param[in] name Full pathname, cannot be NULL.
	\param[out] p	Extracted path name, with directory.  Pass NULL if you do not need it.
	\param[out] f	Extracted filename, without extension.  Pass NULL if you do not need it.
	\param[out] e	Extracted extension.  Pass NULL if you do not need it.
	\todo const-correct the CStr& name argument of SplitFilename
!*/
/*! \brief SplitFilename (CStr* version)
	\ingroup SplitFilename */
UtilExport void SplitFilename(/* const (SDK-Break) */ CStr& name, CStr* p, CStr* f, CStr* e);
/*! \brief SplitFilename (char* version) 
	\ingroup SplitFilename */
UtilExport void SplitFilename(LPCSTR& name, LPSTR p, LPSTR f, LPSTR e);
/*! \brief SplitFilename (WStr* version) 
	\ingroup SplitFilename */
UtilExport void SplitFilename(const WStr& name, WStr* p, WStr* f, WStr* e);
/*! \brief SplitFilename (wchar_t* version) 
	\ingroup SplitFilename */
UtilExport void SplitFilename(LPCWSTR name, LPWSTR p, LPWSTR f, LPWSTR e);

/*--------------------------------------------------
Split filename "name" into 
	p  path
	f  filename.ext
-------------------------------------------------*/

UtilExport void SplitPathFile(TSTR& name,TSTR* p, TSTR* f);


/*--------------------------------------------------
Check to see if s matches the pattern in ptrn
-------------------------------------------------*/

UtilExport BOOL MatchPattern(TSTR &s, TSTR &ptrn, BOOL ignoreCase=TRUE);


//-------------------------------------------------------------------------
// A Case Sensitive "smart" alphanumeric compare that sorts things so that
// numerical suffices come out in numerical order.
//-------------------------------------------------------------------------
UtilExport int MaxAlphaNumComp(MCHAR *a, MCHAR *b);


//-------------------------------------------------------------------------
// A Case Insensitive "smart" alphanumeric compare that sorts things so that
// numerical suffices come out in numerical order.
//-------------------------------------------------------------------------
UtilExport int MaxAlphaNumCompI(MCHAR *a, MCHAR *b);

#endif
