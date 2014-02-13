#pragma once

#include <Furrovine++/Vector2.h>
#include <Furrovine++/Vector3.h>
#include <Furrovine++/Vector4.h>
#include <Furrovine++/TNormal2.h>
#include <Furrovine++/TNormal3.h>
#include <Furrovine++/TNormal4.h>

namespace Fur = Furrovine;

typedef float real;

typedef Fur::TNormal2<real> Norm2;
typedef Fur::TNormal3<real> Norm3;
typedef Fur::TNormal4<real> Norm4;
typedef Fur::TVector2<real> Vec2;
typedef Fur::TVector3<real> Vec3;
typedef Fur::TVector4<real> Vec4;
typedef Fur::RVector2<real> RVec2;
typedef Fur::RVector3<real> RVec3;
typedef Fur::RVector4<real> RVec4;
