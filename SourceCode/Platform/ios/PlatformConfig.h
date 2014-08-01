#ifndef PLATFORMCONFIG_INCLUDE
#define PLATFORMCONFIG_INCLUDE

#include "PlatformDefine.h"

#ifdef BEYONDENGINE_PLATFORM
#undef BEYONDENGINE_PLATFORM
#endif

#define BEYONDENGINE_PLATFORM PLATFORM_IOS

#define BEYONDENGINE_UNUSED_PARAM(unusedparam) (void)unusedparam

#define TOUCH_MAX_NUM 5

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define	glClearDepth				glClearDepthf
#define glDeleteVertexArrays		glDeleteVertexArraysOES
#define glGenVertexArrays			glGenVertexArraysOES
#define glBindVertexArray			glBindVertexArrayOES
#define glMapBuffer					glMapBufferOES
#define glUnmapBuffer				glUnmapBufferOES

#define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES
#define GL_WRITE_ONLY				GL_WRITE_ONLY_OES

#include <sys/time.h>

#endif