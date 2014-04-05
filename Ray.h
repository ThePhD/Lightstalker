#pragma once

#include "real.h"
#include <Furrovine++/Ray3.h>
#include <Furrovine++/optional.h>

template <typename T>
Fur::TVector3<T> reflect( const Fur::RVector3<T>& incident, const Fur::RVector3<T>& normal ) {
	const T cosi = -Fur::dot( normal, incident );
	return incident + ( static_cast<T>( 2 ) * cosi * normal );
}

template <typename T>
Fur::optional<Fur::TVector3<T>> refract( const Fur::RVector3<T>& incident, const Fur::RVector3<T>& normal, T iorn1, T iorn2 ) {
	const T iorratio = iorn1 / iorn2;
	const T cosi = -Fur::dot( normal, incident );
	const T sintsquared = ( iorratio * iorratio ) * ( static_cast<T>( 1 ) - ( cosi * cosi ) );
	if ( sintsquared > static_cast<T>( 1 ) )
		return Fur::nullopt;
	const T cost = std::sqrt( static_cast<T>( 1 ) - sintsquared );
	return ( iorratio * incident ) + ( ( ( iorratio * cosi ) - cost ) * normal );
}

template <typename T>
std::pair<T, T> fresnel( const Fur::RVector3<T>& incident, const Fur::RVector3<T>& normal, T iorn1, T iorn2 ) {
	const T iorratio = iorn1 / iorn2;
	const T cosi = -Fur::dot( normal, incident );
	const T sintsquared = ( iorratio * iorratio ) * ( static_cast<T>( 1 ) - ( cosi * cosi ) );
	if ( sintsquared > static_cast<T>( 1 ) )
		return std::pair<T, T>( static_cast<T>( 1 ), static_cast<T>( 0 ) );
	const T cost = std::sqrt( static_cast<T>( 1 ) - sintsquared );
	const T rorth = ( iorn1 * cosi - iorn2 * cost ) / ( iorn1 * cosi + iorn2 * cost );
	const T rpar = ( iorn2 * cosi - iorn1 * cost ) / ( iorn2 * cosi + iorn1 * cost );
	const T refl = ( rorth * rorth + rpar * rpar ) / static_cast<T>( 2 );
	return std::pair<T, T>( refl, static_cast<T>( 1 ) - refl );
}

template <typename T>
std::pair<T, T> shlick_fresnel( const Fur::RVector3<T>& incident, const Fur::RVector3<T>& normal, T iorn1, T iorn2 ) {
	const T r0 = ( iorn1 - iorn2 ) / ( iorn1 + iorn2 );
	r0 *= r0;
	T cosx = -Fur::dot( normal, incident );
	if ( iorn1 > iorn2 ) {
		const T iorratio = iorn1 / iorn2;
		const T sintsquared = ( iorratio * iorratio ) * ( static_cast<T>( 1 ) - ( cosi * cosi ) );
		if ( sintsquared > static_cast<T>( 1 ) )
			return std::pair<T, T>( static_cast<T>( 1 ), static_cast<T>( 0 ) );
		cosx = std::sqrt( static_cast<T>( 1 ) - sintsquared );
	}
	const T x = static_cast<T>( 1 ) - cosx;
	const T refl = r0 + ( static_cast<T>( 1 ) - r0 ) * x * x * x * x * x;
	return std::pair<T, T>( refl, static_cast<T>( 1 ) - refl );
}


typedef Fur::TRay3<real> Ray;
