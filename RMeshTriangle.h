#pragma once

#include "real.h"
#include <Furrovine++/Vector2.h>
#include <Furrovine++/Vector3.h>
#include <Furrovine++/TVertexTriangle.h>
#include <Furrovine++/THit3.h>

template <typename T>
struct RMeshVertex {
	Fur::RVector3<T> position;
	Fur::RVector3<T> normal;
	Fur::RVector2<T> texture;
};

template <typename T>
struct RMeshTriangle : public Fur::TVertexTriangle<RMeshVertex<T>> {

	Fur::RVector3<T> center( ) const {
		Fur::RVector3<T> cent( a.position );
		cent += b.position;
		cent += c.position;
		cent /= 3;
		return cent;
	}

	Fur::TVector3<T> normal( const Fur::THit3<T>& hit ) const {
		return a.normal + hit.stu.s * ( b.normal - a.normal ) + hit.stu.t * ( c.normal - a.normal );
	}

	Fur::TVector2<T> texture( const Fur::THit3<T>& hit ) const {
		return a + hit.stu.s * ( b - a ) + hit.stu.t * ( c - a );
	}

};
