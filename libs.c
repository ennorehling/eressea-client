#if !defined(_CRT_SECURE_NO_WARNINGS) && defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#define STB_DS_IMPLEMENTATION
#define STB_LIB_IMPLEMENTATION

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4244 4267 4100 4456 4701)
#endif
#include "stb/tests/prerelease/stb_lib.h"
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "stb/stb_ds.h"
