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

	TMaterial( const Fur::TRgba<T>& diffusecolor = Fur::Colors::Red, 
		const Fur::TRgba<T>& specularcolor = Fur::Colors::White, 
		const Fur::TRgba<T>& emissioncolor = Fur::Colors::Transparent, 
		const Fur::TRgba<T>& transmissioncolor = Fur::Colors::Transparent ) {
		diffuse = diffusecolor;
		specular = specularcolor;
		emission = emissioncolor;
		transmission = transmissioncolor;
	}

};

typedef TMaterial<real> Material;
