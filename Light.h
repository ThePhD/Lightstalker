#pragma once

#include "real.h"

template <typename T>
struct TAmbientLight {
	Fur::TRgba<T> intensity;

	TAmbientLight( const Fur::RRgba<T>& intense ) {
		intensity = intense;
	}
};

template <typename T>
struct TDirectionalLight {
	Fur::TVector3<T> direction;
	Fur::TRgba<T> intensity;
	
	TDirectionalLight( const Fur::RVector3<T>& dir = Fur::TVector3<T>::Down, const Fur::RRgba<T>& intense = Fur::TRgba<T>{ 255, 255, 255, 255 } ) {
		direction = dir;
		intensity = intense;
	}
};

template <typename T>
struct TPointLight {
	Fur::TVector3<T> position;
	Fur::TRgba<T> intensity;
	std::size_t primitive;

	TPointLight( const Fur::RVector3<T>& pos = Fur::TVector3<T>::Down, const Fur::RRgba<T>& intense = Fur::TRgba<T>{ 255, 255, 255, 255 } ) : primitive( 0 ) {
		position = pos;
		intensity = intense;
	}
};

typedef TAmbientLight<real> AmbientLight;
typedef TPointLight<real> PointLight;
typedef TDirectionalLight<real> DirectionalLight;
