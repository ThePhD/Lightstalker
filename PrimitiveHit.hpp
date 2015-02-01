#pragma once

#include "Primitive.hpp"
#include "Material.hpp"

template <typename T>
using TPrimitiveHit = Fur::triad<const TPrimitive<T>&, TPrecalculatedMaterial<T>, Fur::THit3<T>>; 

typedef TPrimitiveHit<real> PrimitiveHit;
