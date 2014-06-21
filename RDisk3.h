#pragma once

#include "real.h"
#include <Furrovine++/RDisk3.h>

template <typename T>
struct RDisk3 : Fur::RDisk3<T> {
	Fur::RVector3<T> planareastaxis;
	Fur::RVector3<T> planarnorthaxis;

	Fur::TVector2<T> texture( const Fur::THit3<T>& hit ) {
		return{ hit.contact.dot( planareastaxis ), hit.contact.dot( planarnorthaxis ) };
	}
};
