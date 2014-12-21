\#include "stdafx.h"
\#include "${out_file}.hpp"
#if $macro_judgement
$macro_judgement
#end if 
#for header in $headers
\#include "${header}"
#end for
\#include "LuaBasicConversions.h"
#if $cpp_headers
#for header in $cpp_headers
\#include "${header}"
#end for
#end if 
\#pragma warning(disable:4505) //to disable that the finalize function never be referenced.

