#pragma once

#include "real.h"
#include <Furrovine++/fixed_vector.h>

struct Sample {
	real pixelx;
	real pixely;
	real deltax;
	real deltay;
	real samplex;
	real sampley;
};

const std::size_t n  = 4;
typedef Fur::fixed_vector<Fur::fixed_vector<Sample, n>, n> SampleBlock;
