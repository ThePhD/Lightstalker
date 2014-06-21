#pragma once

#include "real.h"
#include <Furrovine++/RPlane.h>

template <typename T>
struct RPlane : Fur::RPlane<T> {
	Fur::RVector3<T> planareastaxis;
	Fur::RVector3<T> planarnorthaxis;

	Fur::TVector2<T> texture( const Fur::THit3<T>& hit ) {
		return{ hit.contact.dot( planareastaxis ), hit.contact.dot( planarnorthaxis ) };
	}
};
