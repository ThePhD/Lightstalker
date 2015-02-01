#pragma once

#include "real.hpp"
#include "rgba.hpp"
#include "RayBounce.hpp"

class Output {
public:
	virtual void operator()( std::size_t x, std::size_t y, const RayBounce& pixel ) { }
	virtual void Clear( ) { }
	virtual ~Output () { }
};

class NullOutput : public Output { };
