#pragma once

#include "real.h"
#include <Furrovine++/optional.h>

struct sphere_arg_t { };
const auto sphere_arg = sphere_arg_t{ };
struct plane_arg_t { };
const auto plane_arg = plane_arg_t{ };
struct triangle_arg_t { };
const auto triangle_arg = triangle_arg_t{ };

enum class PrimitiveId {
	Sphere,
	Triangle,
	Plane
};

template <typename T>
struct RSphere {
	T radius;
	Fur::RVector3<T> position;
};

template <typename T>
struct RPlane {
	T distance;
	Fur::RVector3<T> normal;
};

template <typename T>
struct RTriangle {
	Fur::RVector3<T> a, b, c;
};

template <typename T>
struct TPrimitive {
	PrimitiveId id;
	union {
		RSphere<T> sphere;
		RPlane<T> plane;
		RTriangle<T> triangle;
	};

	TPrimitive( const RSphere<T>& sphere ) : id( PrimitiveId::Sphere ), sphere( sphere ) {

	}

	TPrimitive( const RPlane<T>& plane ) : id( PrimitiveId::Plane ), plane( plane ) {

	}

	TPrimitive( const RTriangle<T>& triangle ) : id( PrimitiveId::Triangle ), triangle( triangle ) {

	}

	TPrimitive( sphere_arg_t, T radius, const Fur::RVector3<T>& position ) : id( PrimitiveId::Sphere ), sphere( { radius, position } ) {

	}

	TPrimitive( plane_arg_t, T distance, const Fur::RVector3<T>& normal ) : id( PrimitiveId::Plane ), plane( { distance, normal } ) {

	}

	TPrimitive( triangle_arg_t, const Fur::RVector3<T>& a, const Fur::RVector3<T>& b, const Fur::RVector3<T>& c ) : id( PrimitiveId::Triangle ), triangle( { a, b, c } ) {

	}

};

typedef RPlane<real> Plane;
typedef RTriangle<real> Triangle;
typedef RSphere<real> Sphere;
typedef TPrimitive<real> Primitive;
