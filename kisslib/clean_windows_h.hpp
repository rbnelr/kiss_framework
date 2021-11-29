#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1
#endif
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
	#define WIN32_NOMINMAX
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include "windows.h"

#undef near
#undef far
#undef min
#undef max
#undef BF_BOTTOM
#undef BF_TOP
#undef ERROR
