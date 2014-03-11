#pragma once

#include "real.h"
#include "RealRgba.h"

struct Output {
	virtual void Set( real x, real y, const RealRgba& pixel ) { }
	virtual ~Output () { };
};

class NullOutput : public Output { };
