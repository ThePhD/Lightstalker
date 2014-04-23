#pragma once

#include "real.h"
#include "rgba.h"

class Output {
public:
	virtual void Set( real x, real y, const rgba& pixel ) { }
	virtual ~Output () { };
};

class NullOutput : public Output { };
