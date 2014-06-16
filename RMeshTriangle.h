#pragma once

#include "real.h"
#include <Furrovine++/Vector2.h>
#include <Furrovine++/Vector3.h>
#include <Furrovine++/TVertexTriangle.h>

template <typename T>
struct RMeshVertex {
	Fur::RVector3<T> position;
	Fur::RVector3<T> normal;
	Fur::RVector2<T> texture;
};

template <typename T>
struct RMeshTriangle : public Fur::TVertexTriangle<RMeshVertex<T>> {

};
