#pragma once

#include "rgba.h"
#include "Ior.h"
#include "Absorption.h"
#include <Furrovine++/Colors.h>

template <typename T>
struct RMaterial {
	Fur::RRgba<T> color;
	Fur::RRgba<T> ambient;
	Fur::RRgba<T> diffuse;
	Fur::RRgba<T> specular;
	Fur::RRgba<T> refractivity;
	Fur::RRgba<T> reflectivity;
	Fur::RRgba<T> emissive;
	T specularpower;
	T indexofrefraction;
	T absorption;
};


template <typename T>
struct TMaterial : public RMaterial<T> {

	TMaterial( const Fur::TRgba<T>& materialcolor = Fur::Colors::Red,
	const Fur::TRgba<T>& diffuseshade = Fur::Colors::White,
	const Fur::TRgba<T>& specularshade = Fur::Colors::White, 
	T specularpow = static_cast<T>( 255 ),
	const Fur::TRgba<T>& refraction = Fur::Colors::Transparent,
	const Fur::TRgba<T>& reflection = Fur::Colors::Transparent,
	const Fur::TRgba<T>& emission = Fur::Colors::Transparent,
	T indexofrefrac = Ior::Water,
	T absorb = Absorption::Water,
	const Fur::TRgba<T>& ambience = Fur::Colors::White ) {
		color = materialcolor;
		diffuse = diffuseshade;
		specular = specularshade;
		reflectivity = reflection;
		refractivity = refraction;
		specularpower = specularpow;
		emissive = emission;
		ambient = ambience;
		indexofrefraction = indexofrefrac;
		absorption = absorb;
	}

};

typedef TMaterial<real> Material;
