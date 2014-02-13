#pragma once

#include "real.h"
#include "Ray.h"
#include <Furrovine++/unreachable.h>
#include <Furrovine++/optional.h>
#include <Furrovine++/THit.h>

struct sphere_arg_t { };
const auto sphere_arg = sphere_arg_t{ };
struct plane_arg_t { };
const auto plane_arg = plane_arg_t{ };
struct disk_arg_t { };
const auto disk_arg = disk_arg_t{ };
struct triangle_arg_t { };
const auto triangle_arg = triangle_arg_t{ };

enum class PrimitiveId {
	Sphere,
	Triangle,
	Plane,
	Disk
};

template <typename T>
struct TPrimitive {
	std::size_t material;
	PrimitiveId id;
	union {
		Fur::RSphere<T> sphere;
		Fur::RPlane<T> plane;
		Fur::RTriangle<T> triangle;
		Fur::RDisk<T> disk;
	};

	TPrimitive( const Fur::RSphere<T>& sphere ) : id( PrimitiveId::Sphere ), sphere( sphere ) {

	}

	TPrimitive( const Fur::RPlane<T>& plane ) : id( PrimitiveId::Plane ), plane( plane ) {

	}

	TPrimitive( const Fur::RTriangle<T>& triangle ) : id( PrimitiveId::Triangle ), triangle( triangle ) {

	}

	TPrimitive( const Fur::RDisk<T>& disk ) : id( PrimitiveId::Disk ), disk( disk ) {

	}

	TPrimitive( sphere_arg_t, T radius, const Fur::RVector3<T>& position ) : id( PrimitiveId::Sphere ), sphere( { radius, position } ) {

	}

	TPrimitive( plane_arg_t, T distance, const Fur::RVector3<T>& normal ) : id( PrimitiveId::Plane ), plane( { distance, normal } ) {

	}

	TPrimitive( disk_arg_t, T radius, const Fur::RVector3<T>& position, const Fur::RVector3<T>& normal ) : id( PrimitiveId::Plane ), disk( { position.Length( ), normal }, radius, position ) {

	}

	TPrimitive( triangle_arg_t, const Fur::RVector3<T>& a, const Fur::RVector3<T>& b, const Fur::RVector3<T>& c ) : id( PrimitiveId::Triangle ), triangle( { a, b, c } ) {

	}

};

template <typename T>
Fur::optional<Fur::THit<T>> intersect( const Fur::TRay3<T>& ray, const TPrimitive<T>& target ) {
	switch ( target.id ) {
	case PrimitiveId::Plane:
		return Fur::intersect( ray, target.plane );
	case PrimitiveId::Sphere:
		return Fur::intersect( ray, target.sphere );
	case PrimitiveId::Triangle:
		return Fur::intersect( ray, target.triangle );
	case PrimitiveId::Disk:
		return Fur::intersect( ray, target.disk );
	}
	unreachable;
}

typedef Fur::RPlane<real> Plane;
typedef Fur::RTriangle<real> Triangle;
typedef Fur::RSphere<real> Sphere;
typedef TPrimitive<real> Primitive;
typedef Fur::THit<real> Hit;