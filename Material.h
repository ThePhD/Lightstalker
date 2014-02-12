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

	TMaterial( const Fur::RRgba<T>& diffusecolor, const Fur::RRgba<T>& specularcolor, const Fur::RRgba<T>& emissioncolor, const Fur::RRgba<T>& transmissioncolor ) {
		diffuse = diffusecolor;
		specular = specularcolor;
		emission = emissioncolor;
		transmission = transmissioncolor;
	}

};

typedef TMaterial<real> Material;
