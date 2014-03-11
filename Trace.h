#pragma once

#include "Primitive.h"
#include "RealRgba.h"
#include <Furrovine++/optional.h>
#include <vector>

struct Trace {
public:
	Fur::optional<PrimitiveHit> closesthit;
	std::vector<PrimitiveHit> hits;
	RealRgba color;
};