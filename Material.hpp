#pragma once

#include "TMaterial.hpp"
#include <Furrovine++/Graphics/material.hpp>

template <typename T>
struct TBasicMaterial {

	Fur::TRgba<T> matcolor;
	Fur::TRgba<T> matambient;
	Fur::TRgba<T> matdiffuse;
	Fur::TRgba<T> matspecular;
	Fur::TRgba<T> matrefractivity;
	Fur::TRgba<T> matreflectivity;
	Fur::TRgba<T> matemissive;
	Fur::TVector2<T> matstscale;
	T matspecularpower;
	T matindexofrefraction;
	T matabsorption;

	TBasicMaterial( const Fur::TRgba<T>& materialcolor = Fur::TRgba<T>::White,
		const Fur::TRgba<T>& diffuseshade = Fur::TRgba<T>::White,
		const Fur::TRgba<T>& specularshade = Fur::TRgba<T>::White,
		T specularpow = static_cast<T>( 255 ),
		const Fur::TRgba<T>& refraction = Fur::TRgba<T>::White,
		const Fur::TRgba<T>& reflection = Fur::TRgba<T>::Transparent,
		const Fur::TRgba<T>& emission = Fur::TRgba<T>::Transparent,
		T indexofrefrac = Ior::Water,
		T absorb = Absorption::Water,
		const Fur::TRgba<T>& ambience = Fur::TRgba<T>::White,
		const Fur::TVector2<T>& stscaling = Fur::TVector2<T>( static_cast<T>( 1 ), static_cast<T>( 1 ) ) ) {
		matcolor = materialcolor;
		matdiffuse = diffuseshade;
		matspecular = specularshade;
		matreflectivity = reflection;
		matrefractivity = refraction;
		matspecularpower = specularpow;
		matemissive = emission;
		matambient = ambience;
		matstscale = stscaling;
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

	Fur::TVector2<T> stscale( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matstscale;
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

	Fur::TRgba<T> offcolor;
	
	template <typename... Tn>
	TGridMaterial( const Fur::TRgba<T>& offcolor = Fur::TRgba<T>::White,
		Tn&&... argn ) :
		TBasicMaterial( std::forward<Tn>( argn )... ),
		offcolor( offcolor ) {

	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		auto origin = primitive.origin( );
		auto planarvec = Fur::unnormalized_direction_to( origin, hit.contact );
		auto gridsize = stscale( primitive, hit );
		Fur::TSpherical<T> spherical = hit.normal;
		spherical.elevation += Fur::half_pi<T>( );
		std::array<Fur::TVector3<T>, 2> orthogonal = { spherical.cartesian( ), { } };
		orthogonal[ 1 ] = Fur::cross( orthogonal[ 0 ], hit.normal );
		for ( std::size_t i = 0; i < 2; ++i ) {
			auto dist = Fur::distance_to( Fur::project( planarvec, orthogonal[ i ] ), Fur::TVector3<T>::Zero );
			dist += gridsize[ i ] / 2;
			if ( std::fmod( dist, gridsize[ i ] * 2 ) < gridsize[ i ] )
				return offcolor;
		}
		return matcolor;
	}

};

template <typename T>
struct TCheckerMaterial : public TBasicMaterial<T> {

	Fur::TRgba<T> offcolor;
	
	template <typename... Tn>
	TCheckerMaterial( const Fur::TRgba<T>& offcolor = Fur::TRgba<T>::White,
		Tn&&... argn ) :
		TBasicMaterial( std::forward<Tn>( argn )... ),
		offcolor( offcolor ) {

	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		auto origin = primitive.origin( hit );
		auto planarvec = Fur::unnormalized_direction_to( origin, hit.contact );
		auto gridsize = stscale( primitive, hit );
		Fur::TSpherical<T> spherical = hit.normal;
		spherical.elevation += Fur::half_pi<T>( );
		auto sphericalcartesian = spherical.cartesian( );
		std::array<Fur::TVector3<T>, 2> orthogonal = { 
			sphericalcartesian, 
			Fur::cross( sphericalcartesian, hit.normal ) 
		};
		std::array<bool, 2> altchecker{ };
		for ( std::size_t i = 0; i < altchecker.size(); ++i ) {
			auto dist = Fur::distance_to( Fur::project( planarvec, orthogonal[ i ] ), Fur::TVector3<T>::Zero );
			dist += gridsize[ i ] / 2;
			altchecker[ i ] = std::fmod( dist, gridsize[ i ] * 2 ) < gridsize[ i ];
		}
		return altchecker[0] ^ altchecker[1] ? matcolor : offcolor;
	}

};

template <typename T>
struct TWavefrontMaterial {

	Fur::Graphics::material mat;
	Fur::TRgba<T> matreflectivity;

	template <typename... Tn>
	TWavefrontMaterial( Tn&&... argn ) : mat( std::forward<Tn>( argn )... ) {
		T mr = static_cast<T>( mat.Reflectivity );
		matreflectivity = Fur::TRgba<T>( mr, mr, mr, mr );
	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Diffuse;
	}

	Fur::TRgba<T> ambient( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Ambience;
	}

	Fur::TRgba<T> diffuse( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Diffuse;
	}

	Fur::TRgba<T> specular( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Specular;
	}

	Fur::TRgba<T> refractivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Alpha;
	}

	Fur::TRgba<T> reflectivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matreflectivity;
	}

	Fur::TRgba<T> emissive( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.Emissive;
	}

	Fur::TVector2<T> stscale( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return Fur::TVector2<T>( static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	T specularpower( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return static_cast<T>( mat.Shininess );
	}

	T indexofrefraction( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return mat.IndexOfRefraction;
	}

	T absorption( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return Absorption::Vacuum;
	}

};

typedef TMaterial<real> Material;
typedef TBasicMaterial<real> BasicMaterial;
typedef TGridMaterial<real> GridMaterial;
typedef TCheckerMaterial<real> CheckerMaterial;
typedef TPrecalculatedMaterial<real> PrecalculatedMaterial;
typedef TWavefrontMaterial<real> WavefrontMaterial;
