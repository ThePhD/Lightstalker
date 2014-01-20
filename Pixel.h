#pragma once

#include "real.h"

template <typename T>
struct TPixel {
	T r;
	T g;
	T b;
	T a;
};

typedef TPixel<real> Pixel;