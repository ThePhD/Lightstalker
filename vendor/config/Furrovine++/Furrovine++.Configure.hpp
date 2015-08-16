#pragma once

#include <Furrovine++/Platform.OS.hpp>

namespace Furrovine {

#ifdef FURROVINEWIN
#ifdef FURROVINEMINGW
#define FURROVINEOPENGL 1
#define FURROVINEOPENAL 1
#define FURROVINEXINPUT 1
#define FURROVINEFREETYPE 1
// Gdiplus is inferior to FreeType; DirectWrite is pretty great though (but support for it is not quite there yet)
// DirectWrite covers more than FreeType does, including glyph substitutions and the like at times
//#define FURROVINEGDIPLUS 1
//#define FURROVINEDIRECTWRITE 1
#define FURROVINEMATRIXCOLUMNMAJOR 1
#define FURROVINESHADERMATRIXCOLUMNMAJOR 1
#else
//#define FURROVINEDIRECTX 1
#define FURROVINEOPENGL 1
#define FURROVINEXAUDIO 1
#define FURROVINEXINPUT 1
#define FURROVINEFREETYPE 1
#define FURROVINEMATRIXROWMAJOR 1
#define FURROVINESHADERMATRIXCOLUMNMAJOR 1
#endif // MinGW (GCC) cannot handle DirectX properly quite yet
#else
#define FURROVINEOPENGL 1
#define FURROVINEOPENAL 1
#define FURROVINEFREETYPE 1
#define FURROVINEMATRIXCOLUMNMAJOR 1
#define FURROVINESHADERMATRIXCOLUMNMAJOR 1
#endif // WIN

}
