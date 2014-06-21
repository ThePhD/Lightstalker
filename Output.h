#pragma once

#include "real.h"
#include "rgba.h"
#include "RayBounce.h"

class Output {
public:
	virtual void operator()( std::size_t x, std::size_t y, const RayBounce& pixel ) { }
	virtual void Clear( ) { }
	virtual ~Output () { }
};

class NullOutput : public Output { };
