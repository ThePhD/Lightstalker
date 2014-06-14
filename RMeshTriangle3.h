#include <Furrovine++/RTriangle3.h>

template <typename T>
struct RMeshTriangle3 : public Fur::RTriangle3<T> {
	Fur::RVector2<T> ta, tb, tc;
	Fur::RVector3<T> na, nb, nc;

	Fur::RVector3<T> normal( const Fur::RVector3<T>& uvw ) const {
		return normalize( uvw[ 0 ] * na + uvw[ 1 ] * nb + uvw[ 2 ] * nc );
	}

	Fur::RVector2<T> texture( const Fur::RVector3<T>& uvw ) const {
		return normalize( uvw[ 0 ] * ta + uvw[ 1 ] * tb + uvw[ 2 ] * tc );
	}
};