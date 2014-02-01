#pragma once

#include "rgba.h"

template <typename T>
struct RMaterial {
	Fur::RRgba<T> Diffuse;
	Fur::RRgba<T> Specular;
	Fur::RRgba<T> Emission;
	Fur::RRgba<T> Transmission;
};

typedef RMaterial<real> Material;