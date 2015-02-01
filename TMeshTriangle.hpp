#pragma once

#include "real.hpp"
#include <Furrovine++/Vector2.hpp>
#include <Furrovine++/Vector3.hpp>
#include <Furrovine++/TVertexTriangle.hpp>
#include <Furrovine++/THit3.hpp>

template <typename T>
struct TMeshVertex {
	Fur::TVector3<T> position;
	Fur::TVector3<T> normal;
	Fur::TVector2<T> texture;
};

template <typename T>
struct TMeshTriangle : public Fur::TVertexTriangle<TMeshVertex<T>> {

	Fur::TVector3<T> center( ) const {
		Fur::TVector3<T> cent( a.position );
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
