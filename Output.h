#pragma once

#include "real.h"
#include "rgba.h"

class Output {
public:
	virtual void operator()( std::size_t x, std::size_t y, const rgba& pixel ) { }
	virtual void Clear( ) { }
	virtual ~Output () { }
};

class NullOutput : public Output { };
