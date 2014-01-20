#pragma once

#include "real.h"
#include "Pixel.h"

struct Output {
	virtual void Set( real x, real y, const Pixel& pixel ) { }
};

class NullOutput : public Output { };