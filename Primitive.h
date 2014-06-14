#pragma once

#include "real.h"
#include "Ray.h"
#include "Vacuum.h"
#include "RMeshTriangle3.h"
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
struct mesh_triangle_arg_t { };
const auto mesh_triangle_arg = mesh_triangle_arg_t{ };
struct vacuum_arg_t { };
const auto vacuum_arg = vacuum_arg_t{ };

enum class PrimitiveId {
	Vacuum,
	Sphere,
	Triangle,
	MeshTriangle,
	Plane,
	Disk
};


template <typename T>
struct TPrimitive {
	PrimitiveId id;
	std::size_t material;
	bool light;
	union {
		Vacuum vacuum;
		Fur::RSphere<T> sphere;
		Fur::RPlane<T> plane;
		Fur::RDisk3<T> disk;
		Fur::RTriangle3<T> triangle;
		RMeshTriangle3<T> meshtriangle;
	};

	TPrimitive( const Fur::RSphere<T>& sphere ) : id( PrimitiveId::Sphere ), material( 0 ), light( false ), sphere( sphere ) {

	}

	TPrimitive( const Fur::RPlane<T>& plane ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), plane( plane ) {

	}

	TPrimitive( const Fur::RTriangle3<T>& triangle ) : id( PrimitiveId::Triangle ), material( 0 ), light( false ), triangle( triangle ) {

	}

	TPrimitive( const RMeshTriangle3<T>& meshtriangle ) : id( PrimitiveId::MeshTriangle ), material( 0 ), light( false ), meshtriangle( meshtriangle ) {

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

	Fur::TVector3<T> origin( ) const {
		switch ( id ) {
		case PrimitiveId::Plane:
			return plane.distance * plane.normal;
		case PrimitiveId::Sphere:
			return sphere.origin;
		case PrimitiveId::Triangle:
			return triangle.center( );
		case PrimitiveId::MeshTriangle:
			return triangle.center( );
		case PrimitiveId::Disk:
			return disk.origin;
		case PrimitiveId::Vacuum:
			return { };
		default:
			break;
		};
		unreachable;
	}

};

template <typename T>
Fur::optional<Fur::THit3<T>> intersect( const Fur::TRay3<T>& ray, const RMeshTriangle3<T>& target ) {
	Fur::RVector3<T> edge1( target.b ), edge2( target.c );
	edge1 -= target.a;
	edge2 -= target.a;

	Fur::RVector3<T> crossproduct( cross( ray.direction, edge2 ) );
	T dotproduct = edge1.dot( crossproduct );

	if ( dotproduct == static_cast<T>( 0 ) )
		return Fur::nullopt;

	Fur::THit3<T> hit;
	
	Fur::RVector3<T> vdist( ray.origin - target.a );
	hit.stu.s = vdist.dot( crossproduct ) / dotproduct;
	if ( hit.stu.s < static_cast<T>( 0 ) || hit.stu.s > static_cast<T>( 1 ) )
		return Fur::nullopt;

	Fur::RVector3<T> vrdist( vdist.cross( edge1 ) );

	hit.stu.t = dot( ray.direction, vrdist ) / dotproduct;
	if ( hit.stu.t < static_cast<T>( 0 ) || ( hit.stu.s + hit.stu.t ) > static_cast<T>( 1 ) )
		return Fur::nullopt;
	
	hit.distance0 = edge2.dot( vrdist ) / dotproduct;
	if ( hit.distance0 <= static_cast<T>( 0 ) )
		return Fur::nullopt;
	
	hit.stu.u = static_cast<T>( 1 ) - hit.stu.x - hit.stu.y;
	hit.normal = target.normal( hit.stu );
	hit.contact = ray.at( hit.distance0 );
	hit.inside = false;
	return hit;
}

template <typename T>
Fur::optional<Fur::THit3<T>> intersect( const Fur::TRay3<T>& ray, const TPrimitive<T>& target ) {
	switch ( target.id ) {
	case PrimitiveId::Plane:
		return Fur::intersect( ray, target.plane );
	case PrimitiveId::Sphere:
		return Fur::intersect( ray, target.sphere );
	case PrimitiveId::Triangle:
		return Fur::intersect( ray, target.triangle );
	case PrimitiveId::MeshTriangle:
		return intersect( ray, target.meshtriangle );
	case PrimitiveId::Disk:
		return Fur::intersect( ray, target.disk );
	case PrimitiveId::Vacuum:
		return Fur::THit3<T>{ };
	}
	unreachable;
}

typedef Fur::RPlane<real> Plane;
typedef Fur::RTriangle3<real> Triangle;
typedef RMeshTriangle3<real> MeshTriangle;
typedef Fur::RSphere<real> Sphere;
typedef Fur::RDisk3<real> Disk;
typedef Fur::THit3<real> Hit;
typedef TPrimitive<real> Primitive;
