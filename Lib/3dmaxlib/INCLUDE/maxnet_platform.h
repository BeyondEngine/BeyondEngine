//-----------------------------------------------------------------------------
// ----------------------------
// File ....: maxnet_platform.h
// ----------------------------
// Author...: Gus J Grubba
// Date ....: February 2000
//
// Descr....: 3D Studio MAX Network Interface - Platform Dependency
//
// History .: Feb, 07 2000 - Started
//            
//-----------------------------------------------------------------------------

#ifndef _MAXNET_PLATFORM_H_
#define _MAXNET_PLATFORM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32

#include <windows.h>

#define mn_malloc(s)	LocalAlloc(LPTR,s)
#define mn_free			LocalFree

#else

#define	 __USE_BSD

#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define mn_malloc(s)	malloc(s)
#define mn_free			free

#endif

#endif

//-- EOF: maxnet_platform.h ---------------------------------------------------
