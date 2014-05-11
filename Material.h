#pragma once

#include "TMaterial.h"

template <typename T>
struct TBasicMaterial {

	Fur::RRgba<T> matcolor;
	Fur::RRgba<T> matambient;
	Fur::RRgba<T> matdiffuse;
	Fur::RRgba<T> matspecular;
	Fur::RRgba<T> matrefractivity;
	Fur::RRgba<T> matreflectivity;
	Fur::RRgba<T> matemissive;
	T matspecularpower;
	T matindexofrefraction;
	T matabsorption;

	TBasicMaterial( const Fur::TRgba<T>& materialcolor = Fur::Colors::Red,
		const Fur::TRgba<T>& diffuseshade = Fur::Colors::White,
		const Fur::TRgba<T>& specularshade = Fur::Colors::White,
		T specularpow = static_cast<T>( 255 ),
		const Fur::TRgba<T>& refraction = Fur::Colors::Transparent,
		const Fur::TRgba<T>& reflection = Fur::Colors::Transparent,
		const Fur::TRgba<T>& emission = Fur::Colors::Transparent,
		T indexofrefrac = Ior::Water,
		T absorb = Absorption::Water,
		const Fur::TRgba<T>& ambience = Fur::Colors::White ) {
		matcolor = materialcolor;
		matdiffuse = diffuseshade;
		matspecular = specularshade;
		matreflectivity = reflection;
		matrefractivity = refraction;
		matspecularpower = specularpow;
		matemissive = emission;
		matambient = ambience;
		matindexofrefraction = indexofrefrac;
		matabsorption = absorb;
	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matcolor;
	}

	Fur::TRgba<T> ambient( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matambient;
	}

	Fur::TRgba<T> diffuse( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matdiffuse;
	}

	Fur::TRgba<T> specular( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matspecular;
	}

	Fur::TRgba<T> refractivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matrefractivity;
	}

	Fur::TRgba<T> reflectivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matreflectivity;
	}

	Fur::TRgba<T> emissive( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matemissive;
	}

	T specularpower( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matspecularpower;
	}

	T indexofrefraction( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matindexofrefraction;
	}

	T absorption( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matabsorption;
	}

};

template <typename T>
struct TGridMaterial : public TBasicMaterial<T> {

	Fur::RRgba<T> offcolor;
	T gridsizex;
	T gridsizey;

	TGridMaterial( const Fur::TRgba<T>& materialcolor = Fur::Colors::Red,
		const Fur::TRgba<T>& diffuseshade = Fur::Colors::White,
		const Fur::TRgba<T>& specularshade = Fur::Colors::White,
		T specularpow = static_cast<T>( 255 ),
		const Fur::TRgba<T>& refraction = Fur::Colors::Transparent,
		const Fur::TRgba<T>& reflection = Fur::Colors::Transparent,
		const Fur::TRgba<T>& emission = Fur::Colors::Transparent,
		T indexofrefrac = Ior::Water,
		T absorb = Absorption::Water,
		const Fur::TRgba<T>& ambience = Fur::Colors::White ) :
		TBasicMaterial( materialcolor, diffuseshade, specularshade, specularpow, refraction, reflection, emission, indexofrefrac, absorb, ambience ) {

	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matcolor;
	}

};


typedef TBasicMaterial<real> BasicMaterial;
typedef TGridMaterial<real> GridMaterial;
