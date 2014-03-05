#pragma once

#include "rgba.h"
#include <Furrovine++/Colors.h>

template <typename T>
struct RMaterial {
	Fur::RRgba<T> diffuse;
	Fur::RRgba<T> specular;
	Fur::RRgba<T> emission;
	Fur::RRgba<T> transmission;
	T specularpower;
};


template <typename T>
struct TMaterial : public RMaterial<T> {

	TMaterial( const Fur::TRgba<T>& diffusecolor = Fur::Colors::Red, 
	const Fur::TRgba<T>& specularcolor = Fur::Colors::White, 
	T specularcolorpower = static_cast<T>( 255 ),
	const Fur::TRgba<T>& emissioncolor = Fur::Colors::Transparent, 
	const Fur::TRgba<T>& transmissioncolor = Fur::Colors::Transparent ) {
		diffuse = diffusecolor;
		specular = specularcolor;
		emission = emissioncolor;
		transmission = transmissioncolor;
		specularpower = specularcolorpower;
	}

};

typedef TMaterial<real> Material;
