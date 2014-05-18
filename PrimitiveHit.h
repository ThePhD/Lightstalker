#pragma once

#include "Primitive.h"
#include "Material.h"

template <typename T>
using TPrimitiveHit = Fur::triple<const TPrimitive<T>&, TPrecalculatedMaterial<T>, Fur::THit3<T>>; 

typedef TPrimitiveHit<real> PrimitiveHit;
