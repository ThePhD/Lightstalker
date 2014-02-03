#pragma once

#include "rgba.h"
#include <Furrovine++/Colors.h>

template <typename T>
struct RMaterial {
	Fur::RRgba<T> diffuse;
	Fur::RRgba<T> specular;
	Fur::RRgba<T> emission;
	Fur::RRgba<T> transmission;
};


template <typename T>
struct TMaterial : public RMaterial<T> {

	TMaterial( ) {
		diffuse = Fur::Colors::White;
		specular = Fur::Colors::Transparent;
		emission = Fur::Colors::Transparent;
		transmission = Fur::Colors::Transparent;
	}

};

typedef TMaterial<real> Material;
