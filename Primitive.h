#pragma once

#include "real.h"
#include "Ray.h"
#include <Furrovine++/unreachable.h>
#include <Furrovine++/optional.h>
#include <Furrovine++/THit3.h>
#include <Furrovine++/RSphere.h>
#include <Furrovine++/RTriangle3.h>
#include <Furrovine++/RDisk3.h>
#include <Furrovine++/RPlane.h>
#include <Furrovine++/intersect3.h>
#include <Furrovine++/triple.h>

struct sphere_arg_t { };
const auto sphere_arg = sphere_arg_t{ };
struct plane_arg_t { };
const auto plane_arg = plane_arg_t{ };
struct disk_arg_t { };
const auto disk_arg = disk_arg_t{ };
struct triangle_arg_t { };
const auto triangle_arg = triangle_arg_t{ };
struct vacuum_arg_t { };
const auto vacuum_arg = vacuum_arg_t{ };

enum class PrimitiveId {
	Vacuum,
	Sphere,
	Triangle,
	Plane,
	Disk
};

struct Vacuum { };

template <typename T>
struct TPrimitive {
	PrimitiveId id;
	std::size_t material;
	bool light;
	union {
		Vacuum vacuum;
		Fur::RSphere<T> sphere;
		Fur::RPlane<T> plane;
		Fur::RTriangle3<T> triangle;
		Fur::RDisk3<T> disk;
	};

	TPrimitive( const Fur::RSphere<T>& sphere ) : id( PrimitiveId::Sphere ), material( 0 ), light( false ), sphere( sphere ) {

	}

	TPrimitive( const Fur::RPlane<T>& plane ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), plane( plane ) {

	}

	TPrimitive( const Fur::RTriangle3<T>& triangle ) : id( PrimitiveId::Triangle ), material( 0 ), light( false ), triangle( triangle ) {

	}

	TPrimitive( const Fur::RDisk3<T>& disk ) : id( PrimitiveId::Disk ), material( 0 ), light( false ), disk( disk ) {

	}

	TPrimitive( vacuum_arg_t ) : id( PrimitiveId::Vacuum ), material( 0 ), light( false ), vacuum( ) {

	}

	TPrimitive( sphere_arg_t, T radius, const Fur::RVector3<T>& position ) : id( PrimitiveId::Sphere ), material( 0 ), light( false ), sphere( { radius, position } ) {

	}

	TPrimitive( plane_arg_t, T distance, const Fur::RVector3<T>& normal ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), plane( { distance, normal } ) {

	}

	TPrimitive( disk_arg_t, T radius, const Fur::RVector3<T>& position, const Fur::RVector3<T>& normal ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), disk( { position.Length( ), normal }, radius, position ) {

	}

	TPrimitive( triangle_arg_t, const Fur::RVector3<T>& a, const Fur::RVector3<T>& b, const Fur::RVector3<T>& c ) : id( PrimitiveId::Triangle ), material( 0 ), light( false ), triangle( { a, b, c } ) {

	}

};

template <typename T>
Fur::optional<Fur::THit3<T>> intersect( const Fur::TRay3<T>& ray, const TPrimitive<T>& target ) {
	switch ( target.id ) {
	case PrimitiveId::Plane:
		return Fur::intersect( ray, target.plane );
	case PrimitiveId::Sphere:
		return Fur::intersect( ray, target.sphere );
	case PrimitiveId::Triangle:
		return Fur::intersect( ray, target.triangle );
	case PrimitiveId::Disk:
		return Fur::intersect( ray, target.disk );
	case PrimitiveId::Vacuum:
		return Fur::THit3<T>{ };
	}
	unreachable;
}

typedef Fur::RPlane<real> Plane;
typedef Fur::RTriangle3<real> Triangle;
typedef Fur::RSphere<real> Sphere;
typedef Fur::RDisk3<real> Disk;
typedef Fur::THit3<real> Hit;
typedef TPrimitive<real> Primitive;
