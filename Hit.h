#pragma once

#include <Furrovine++/unreachable.h>
#include "Primitive.h"
#include "Ray.h"

template <typename T>
struct RHit {
	T distance0;
	T distance1;
	Fur::TVector3<T> contact;
	Fur::TVector3<T> normal;
};

template <typename T>
Fur::optional<RHit<T>> intersect( const Fur::TRay3<T>& ray, const RPlane<T>& target ) {
	T denom = ray.direction.Dot( target.normal );
	if ( denom <= 0 )
		return nullopt;

	RHit<T> hit;
	hit.distance0 = ( target.normal.Dot( ray.origin ) + target.distance ) / denom;
	hit.contact = ray.At( hit.distance0 );
	hit.normal = target.normal;
	return hit;
}

template <typename T>
Fur::optional<RHit<T>> intersect( const Fur::TRay3<T>& ray, const RSphere<T>& target ) {
	Fur::TVector3<T> ray2sphere = target.position - ray.origin;
	T dist = ray2sphere.Dot( ray.direction );
	T ray2spheresquared = ray2sphere.Dot( ray2sphere );
	T radiussquared = target.radius * target.radius;

	if ( dist < 0 
		&& ray2spheresquared > radiussquared ) {
		return nullopt;
	}

	T raycrossessphere = ray2spheresquared - dist * dist;

	if ( raycrossessphere > radiussquared ) {
		return nullopt;
	}

	T q = static_cast<T>( sqrt( radiussquared - raycrossessphere ) );

	RHit<T> hit;
	if ( ray2spheresquared > radiussquared )
		hit.distance0 = dist - q;
	else 
		hit.distance0 = dist + q;

	hit.contact = ray.At( hit.distance0 );
	hit.normal = target.position - hit.contact;
	hit.normal.Normalize();

	return hit;
}

template <typename T>
Fur::optional<RHit<T>> intersect( const Fur::TRay3<T>& ray, const RTriangle<T>& target ) {
	return nullopt;
}

template <typename T>
Fur::optional<RHit<T>> intersect( const Fur::TRay3<T>& ray, const TPrimitive<T>& target ) {
	switch ( target.id ) {
	case PrimitiveId::Plane:
		return intersect( ray, target.plane );
	case PrimitiveId::Sphere:
		return intersect( ray, target.sphere );
	case PrimitiveId::Triangle:
		return intersect( ray, target.triangle );
	}
	unreachable;
}

typedef RHit<real> Hit;