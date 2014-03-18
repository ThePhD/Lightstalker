#pragma once

#include "Primitive.h"
#include "RealRgba.h"
#include <Furrovine++/optional.h>
#include <vector>

struct RayTrace {
public:
	Fur::optional<PrimitiveHit> closesthit;
	std::vector<PrimitiveHit> hits;
	std::vector<PrimitiveHit*> orderedhits;
	RealRgba color;
};