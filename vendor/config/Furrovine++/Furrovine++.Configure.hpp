#pragma once

#include <Furrovine++/Platform.OS.hpp>

namespace Furrovine {

#ifdef FURROVINE_WIN
#ifdef FURROVINE_MINGW
#define FURROVINE_OPENGL 1
#define FURROVINE_OPENGL44 1
#define FURROVINE_OPENAL 1
#define FURROVINE_XINPUT 1
#define FURROVINE_FREETYPE 1
// Gdiplus is inferior to FreeType; DirectWrite is pretty great though (but support for it is not quite there yet)
// DirectWrite covers more than FreeType does, including glyph substitutions and the like at times
//#define FURROVINEGDIPLUS 1
//#define FURROVINEDIRECTWRITE 1
#define FURROVINE_MATRIX_COLUMN_MAJOR 1
#define FURROVINE_SHADER_MATRIX_COLUMN_MAJOR 1
#else
#define FURROVINE_DIRECTX 1
//#define FURROVINE_OPENGL 1
//#define FURROVINE_OPENGL44 1
#define FURROVINE_XAUDIO 1
#define FURROVINE_XINPUT 1
#define FURROVINE_FREETYPE 1
#define FURROVINE_MATRIX_ROW_MAJOR 1
#define FURROVINE_SHADER_MATRIX_COLUMN_MAJOR 1
#define FURROVINE_COORDINATE_SYSTEM_LEFT_HANDED 1
//#define FURROVINE_MATRIX_COLUMN_MAJOR 1
//#define FURROVINE_SHADER_MATRIX_COLUMN_MAJOR 1
//#define FURROVINE_COORDINATE_SYSTEM_RIGHT_HANDED 1
#endif // MinGW (GCC) cannot handle DirectX properly quite yet
#else
#define FURROVINE_OPENGL 1
#define FURROVINE_OPENAL 1
#define FURROVINE_FREETYPE 1
#define FURROVINE_MATRIX_COLUMN_MAJOR 1
#define FURROVINE_SHADER_MATRIX_COLUMN_MAJOR 1
#define FURROVINE_COORDINATE_SYSTEM_RIGHT_HANDED 1
#endif // WIN

}
