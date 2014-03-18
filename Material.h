#pragma once

#include "RealRgba.h"
#include "Ior.h"
#include <Furrovine++/Colors.h>

template <typename T>
struct RMaterial {
	Fur::RRgba<T> ambient;
	Fur::RRgba<T> diffuse;
	Fur::RRgba<T> specularity;
	Fur::RRgba<T> reflectivity;
	Fur::RRgba<T> transparency;
	Fur::RRgba<T> emissive;
	T specularpower;
	T indexofrefraction;
};


template <typename T>
struct TMaterial : public RMaterial<T> {

	TMaterial( const Fur::TRgba<T>& diffusecolor = Fur::Colors::Red, 
	const Fur::TRgba<T>& specular = Fur::Colors::White, 
	T specularpow = static_cast<T>( 255 ),
	const Fur::TRgba<T>& reflect = Fur::Colors::Transparent, 
	const Fur::TRgba<T>& transparent = Fur::Colors::Transparent,
	const Fur::TRgba<T>& emission = Fur::Colors::Transparent,
	T indexofrefrac = Ior::Glass,
	const Fur::TRgba<T>& ambience = Fur::Colors::White ) {
		diffuse = diffusecolor;
		specularity = specular;
		reflectivity = reflect;
		transparency = transparent;
		specularpower = specularpow;
		emissive = emission;
		ambient = ambience;
		indexofrefraction = indexofrefrac;
	}

};

typedef TMaterial<real> Material;
