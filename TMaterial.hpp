#pragma once

#include "rgba.hpp"
#include "Ior.hpp"
#include "Absorption.hpp"
#include "Primitive.hpp"
#include <memory>

template <typename T>
struct TMaterial {
	struct base {
		virtual Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> ambient( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> diffuse( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> specular( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> refractivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> reflectivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TRgba<T> emissive( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual Fur::TVector2<T> stscale( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual T specularpower( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual T indexofrefraction( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;

		virtual T absorption( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const = 0;
	};

	template <typename Tm>
	struct interfacer : base {
		typedef typename std::decay<Tm>::type Tdm;
		Tdm m;

		template <typename... Tmm>
		interfacer ( Tmm&&... mm ) : m( std::forward<Tmm>( mm )... ) {

		}

		Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.color( primitive, hit );
		}

		Fur::TRgba<T> ambient( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.ambient( primitive, hit );
		}

		Fur::TRgba<T> specular( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.specular( primitive, hit );
		}

		Fur::TRgba<T> diffuse( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.diffuse( primitive, hit );
		}

		Fur::TRgba<T> refractivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.refractivity( primitive, hit );
		}

		Fur::TRgba<T> reflectivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.reflectivity( primitive, hit );
		}

		Fur::TRgba<T> emissive( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.emissive( primitive, hit );
		}

		Fur::TVector2<T> stscale( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.stscale( primitive, hit );
		}

		T specularpower( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.specularpower( primitive, hit );
		}

		T indexofrefraction( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.indexofrefraction( primitive, hit );
		}

		T absorption( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const override {
			return m.absorption( primitive, hit );
		}
	};

	std::unique_ptr<base> matbase;

	template <typename Tm>
	TMaterial( Tm&& m ) : matbase( std::make_unique<interfacer<typename std::decay<Tm>::type>>( interfacer<typename std::decay<Tm>::type>{ std::forward<Tm>( m ) } ) ) {

	}

	TMaterial( TMaterial&& mov ) : matbase( std::move( mov.matbase ) ) {

	}

	TMaterial& operator=( TMaterial&& mov ) {
		matbase = std::move( mov.matbase );
		return *this;
	}

	Fur::TRgba<T> color( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->color( primitive, hit );
	}

	Fur::TRgba<T> ambient( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->ambient( primitive, hit );
	}

	Fur::TRgba<T> diffuse( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->diffuse( primitive, hit );
	}

	Fur::TRgba<T> specular( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->specular( primitive, hit );
	}

	Fur::TRgba<T> refractivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->refractivity( primitive, hit );
	}

	Fur::TRgba<T> reflectivity( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->reflectivity( primitive, hit );
	}

	Fur::TRgba<T> emissive( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->emissive( primitive, hit );
	}

	Fur::TVector2<T> stscale( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->stscale( primitive, hit );
	}

	T specularpower( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->specularpower( primitive, hit );
	}

	T indexofrefraction( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->indexofrefraction( primitive, hit );
	}

	T absorption( const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) const {
		return matbase->absorption( primitive, hit );
	}

};

template <typename T>
struct TPrecalculatedMaterial {

	Fur::TRgba<T> color;
	Fur::TRgba<T> ambient;
	Fur::TRgba<T> diffuse;
	Fur::TRgba<T> specular;
	Fur::TRgba<T> refractivity;
	Fur::TRgba<T> opacity;
	Fur::TRgba<T> reflectivity;
	Fur::TRgba<T> emissive;
	Fur::TVector2<T> stscale;
	T specularpower;
	T indexofrefraction;
	T absorption;

	TPrecalculatedMaterial( const TMaterial<T>& material, const TPrimitive<T>& primitive, const Fur::THit3<T>& hit ) {
		const static Fur::TRgba<T> whitepoint = Fur::TRgba<T>::White;
		color = material.color( primitive, hit );
		ambient = material.ambient( primitive, hit );
		diffuse = material.diffuse( primitive, hit );
		specular = material.specular( primitive, hit );
		refractivity = material.refractivity( primitive, hit );
		reflectivity = material.reflectivity( primitive, hit );
		emissive = material.emissive( primitive, hit );
		stscale = material.stscale( primitive, hit );
		specularpower = material.specularpower( primitive, hit );
		indexofrefraction = material.indexofrefraction( primitive, hit );
		absorption = material.absorption( primitive, hit );
		opacity = whitepoint - refractivity;
	}

};
