#pragma once

#include "rgba.h"

template <typename T>
struct RMaterial {
	Fur::RRgba<T> diffuse;
	Fur::RRgba<T> specular;
	Fur::RRgba<T> emission;
	Fur::RRgba<T> transmission;
};


template <typename T>
struct TMaterial : RMaterial<T> {

	TMaterial( ) {
		diffuse = Colors::White;
		specular = Colors::Transparent;
		emission = Colors::Transparent;
		transmission = Colors::Transparent;
	}

};

typedef TMaterial<real> Material;