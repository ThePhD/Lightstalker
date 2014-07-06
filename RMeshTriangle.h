#pragma once

#include "real.h"
#include <Furrovine++/Vector2.h>
#include <Furrovine++/Vector3.h>
#include <Furrovine++/RVertexTriangle.h>
#include <Furrovine++/THit3.h>

template <typename T>
struct RMeshVertex {
	Fur::RVector3<T> position;
	Fur::RVector3<T> normal;
	Fur::RVector2<T> texture;
};

template <typename T>
struct RMeshTriangle : public Fur::RVertexTriangle<RMeshVertex<T>> {

	Fur::RVector3<T> center( ) const {
		Fur::RVector3<T> cent( a.position );
		cent += b.position;
		cent += c.position;
		cent /= 3;
		return cent;
	}

	Fur::TVector3<T> normal( const Fur::THit3<T>& hit ) const {
		std::array<Fur::TVector3<T>, 3> normals = {
			a.normal,
			b.normal,
			c.normal
		};
		return barycentric_interpolate( hit.uvw, normals );
	}

	Fur::TVector2<T> texture( const Fur::THit3<T>& hit ) const {
		return a + hit.uvw.u * ( b - a ) + hit.uvw.v * ( c - a );
	}

};
