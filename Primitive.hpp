#pragma once

#include "real.hpp"
#include "Ray.hpp"
#include "Vacuum.hpp"
#include "TMeshTriangle.hpp"
#include "Math.hpp"
#include <Furrovine++/unreachable.hpp>
#include <Furrovine++/optional.hpp>
#include <Furrovine++/THit3.hpp>
#include <Furrovine++/TSphere.hpp>
#include <Furrovine++/TBox.hpp>
#include <Furrovine++/TTriangle3.hpp>
#include <Furrovine++/intersect3.hpp>
#include <Furrovine++/enclose3.hpp>
#include <Furrovine++/triad.hpp>

struct sphere_arg_t { };
const auto sphere_arg = sphere_arg_t{ };
struct plane_arg_t { };
const auto plane_arg = plane_arg_t{ };
struct disk_arg_t { };
const auto disk_arg = disk_arg_t{ };
struct triangle_arg_t { };
const auto triangle_arg = triangle_arg_t{ };
struct vertex_triangle_arg_t { };
const auto vertex_triangle_arg = vertex_triangle_arg_t{ };
struct box_arg_t { };
const auto box_arg = box_arg_t{ };
struct vacuum_arg_t { };
const auto vacuum_arg = vacuum_arg_t{ };

enum class PrimitiveId {
	Vacuum,
	Sphere,
	Triangle,
	VertexTriangle,
	Plane,
	Disk,
	Box
};

template <typename T>
struct TPrimitive {
	PrimitiveId id;
	std::size_t material;
	bool light;
	union {
		Vacuum vacuum;
		Fur::TSphere<T> sphere;
		Fur::TPlane<T> plane;
		Fur::TDisk3<T> disk;
		Fur::TBox<T> box;
		Fur::TTriangle3<T> triangle;
		TMeshTriangle<T> meshtriangle;
	};

	TPrimitive( const Fur::TSphere<T>& sphere ) : id( PrimitiveId::Sphere ), material( 0 ), light( false ), sphere( sphere ) {

	}

	TPrimitive( const Fur::TPlane<T>& plane ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), plane( plane ) {

	}

	TPrimitive( const Fur::TTriangle3<T>& triangle ) : id( PrimitiveId::Triangle ), material( 0 ), light( false ), triangle( triangle ) {

	}

	TPrimitive( const TMeshTriangle<T>& meshtriangle ) : id( PrimitiveId::VertexTriangle ), material( 0 ), light( false ), meshtriangle( meshtriangle ) {

	}

	TPrimitive( const Fur::TDisk3<T>& disk ) : id( PrimitiveId::Disk ), material( 0 ), light( false ), disk( disk ) {

	}

	TPrimitive( const Fur::TBox<T>& box ) : id( PrimitiveId::Box ), material( 0 ), light( false ), box( box ) {

	}

	TPrimitive( vacuum_arg_t ) : id( PrimitiveId::Vacuum ), material( 0 ), light( false ), vacuum( ) {

	}

	TPrimitive( sphere_arg_t, T radius, const Fur::TVector3<T>& position ) : id( PrimitiveId::Sphere ), material( 0 ), light( false ), sphere( ) {
		sphere.radius = radius;
		sphere.origin = position;
	}

	TPrimitive( plane_arg_t, T distance, const Fur::TVector3<T>& normal ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), plane( ) {
		plane.distance = distance;
		plane.normal = normal;
		Fur::TSpherical<T> spherical( plane.normal );
		spherical.elevation += Fur::half_pi<T>( );
		plane.planareastaxis = spherical.cartesian( );
		plane.planarnorthaxis = cross( plane.normal, plane.planareastaxis );
	}

	TPrimitive( disk_arg_t, T radius, const Fur::TVector3<T>& position, const Fur::TVector3<T>& normal ) : id( PrimitiveId::Plane ), material( 0 ), light( false ), disk( { position.Length( ), normal }, radius, position ) {
		disk.distance = distance;
		disk.normal = normal;
		disk.planareastaxis = TVector3<T>( normal.y, normal.z, normal.x );
		disk.planarnorthaxis = disk.planareastaxis.cross( disk.normal );
	}

	TPrimitive( triangle_arg_t, const Fur::TVector3<T>& a, const Fur::TVector3<T>& b, const Fur::TVector3<T>& c ) : id( PrimitiveId::Triangle ), material( 0 ), light( false ), triangle( { a, b, c } ) {

	}

	TPrimitive( box_arg_t, const Fur::TVector3<T>& min, const Fur::TVector3<T>& max ) : id( PrimitiveId::Box ), material( 0 ), light( false ), box( { min, max } ) {

	}

	Fur::TVector3<T> normal( const Fur::THit3<T>& hit ) {
		switch ( id ) {
		case PrimitiveId::Plane:
			return plane.normal;
		case PrimitiveId::Sphere:
			return hit.normal;
		case PrimitiveId::Triangle:
			return hit.normal;
		case PrimitiveId::VertexTriangle:
			return meshtriangle.normal( hit );
		case PrimitiveId::Disk:
			return disk.plane.normal;
		case PrimitiveId::Box:
			return hit.normal;
		case PrimitiveId::Vacuum:
		default:
			return{ };
			break;
		};
	}

	Fur::TVector2<T> texture( const Fur::THit3<T>& hit ) {
		switch ( id ) {
		case PrimitiveId::Plane:
			return Fur::TVector2<T>( hit.contact.dot( plane.planareastaxis ), hit.contact.dot( plane.planarnorthaxis ) );
		case PrimitiveId::Disk:
			return Fur::TVector2<T>( hit.contact.dot( disk.planareastaxis ), hit.contact.dot( disk.planarnorthaxis ) );
		case PrimitiveId::Sphere: {
			Fur::TVector2<T> st(
				( ( ( std::asin( hit.normal.x ) * 2 ) / pi<T>( ) ) + 1 ),
				( ( ( std::asin( hit.normal.y ) * 2 ) / pi<T>( ) ) + 1 )
				);
			st /= 2;
			return st };
		case PrimitiveId::Triangle:
			return hit.stu;
		case PrimitiveId::VertexTriangle:
			return meshtriangle.texture( hit );
		case PrimitiveId::Box:
			return hit.stu;
		case PrimitiveId::Vacuum:
		default:
			return{ };
		};
	}

	Fur::TVector3<T> origin( ) const {
		switch ( id ) {
		case PrimitiveId::Plane:
			return plane.distance * plane.normal;
		case PrimitiveId::Sphere:
			return sphere.origin;
		case PrimitiveId::Triangle:
			return triangle.center( );
		case PrimitiveId::VertexTriangle:
			return meshtriangle.center( );
		case PrimitiveId::Disk:
			return disk.origin;
		case PrimitiveId::Box:
			return box.center( );
		case PrimitiveId::Vacuum:
		default:
			return{ };
		};
	}

	Fur::TVector3<T> origin( const Fur::THit3<T>& hit ) const {
		switch ( id ) {
		case PrimitiveId::Plane:
			return plane.distance * plane.normal;
		case PrimitiveId::Sphere:
			return sphere.origin;
		case PrimitiveId::Triangle:
			return triangle.center( );
		case PrimitiveId::VertexTriangle:
			return meshtriangle.center( );
		case PrimitiveId::Disk:
			return disk.origin;
		case PrimitiveId::Box:
			return box.center( );
		case PrimitiveId::Vacuum:
		default:
			return{ };
		};
	}

	void enclose_by( Fur::TBox<T>& enclosure ) const {
		switch ( id ) {
		case PrimitiveId::Plane:
			return; // Nope
		case PrimitiveId::Sphere:
			enclose( enclosure, sphere );
			break;
		case PrimitiveId::Triangle:
			enclose( enclosure, triangle );
			break;
		case PrimitiveId::VertexTriangle:
			enclose( enclosure, meshtriangle );
		case PrimitiveId::Disk:
			enclose( enclosure, disk );
		case PrimitiveId::Box:
			enclose( enclosure, box );
		case PrimitiveId::Vacuum:
		default:
			return;
		};
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
	case PrimitiveId::VertexTriangle: {
		auto i = Fur::intersect( ray, target.meshtriangle );
		if ( i )
			i->normal = target.meshtriangle.normal( *i );
		return i; }
	case PrimitiveId::Disk:
		return Fur::intersect( ray, target.disk );
	case PrimitiveId::Box:
		return Fur::intersect( ray, target.box );
	case PrimitiveId::Vacuum:
		return Fur::THit3<T>{ };
	}
	unreachable;
}

template <typename T>
Fur::optional<Fur::THit3<T>> intersect( const Fur::TBox<T>& box, const TPrimitive<T>& target ) {
	switch ( target.id ) {
	case PrimitiveId::Plane:
		return Fur::intersect( box, target.plane );
	case PrimitiveId::Sphere:
		return Fur::intersect( box, target.sphere );
	case PrimitiveId::Triangle:
		return Fur::intersect( box, target.triangle );
	case PrimitiveId::VertexTriangle: {
		auto i = Fur::intersect( box, target.meshtriangle );
		if ( i )
			i->normal = target.meshtriangle.normal( *i );
		return i; }
	case PrimitiveId::Disk:
		return Fur::intersect( box, target.disk );
	case PrimitiveId::Box:
		return Fur::intersect( box, target.box );
	case PrimitiveId::Vacuum:
		return Fur::THit3<T>{ };
	}
	unreachable;
}

typedef Fur::TPlane<real> Plane;
typedef Fur::TTriangle3<real> Triangle;
typedef Fur::TSphere<real> Sphere;
typedef Fur::TDisk3<real> Disk;
typedef Fur::TBox<real> Box;
typedef TMeshVertex<real> MeshVertex;
typedef TMeshTriangle<real> MeshTriangle;
typedef Fur::THit3<real> Hit;
typedef TPrimitive<real> Primitive;
