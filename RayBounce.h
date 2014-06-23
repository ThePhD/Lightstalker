#pragma once

#include "real.h"
#include "PrimitiveHit.h"
#include <Furrovine++/optional.h>

struct RayBounce {
	Ray ray;
	vec3u raysign;
	rgba color;
	Fur::optional<PrimitiveHit> hit;
	bool shadow;
	std::uintptr_t hitid;
	std::size_t samples;
	std::size_t reflections;
	std::size_t refractions;
	std::size_t cullingtraversalhits;
	std::size_t primitivetests;
	std::size_t primitivehits;
	std::size_t overlappingprimitivehits;
	
	RayBounce( );

	void accumulate( const RayBounce& bounce );

};
