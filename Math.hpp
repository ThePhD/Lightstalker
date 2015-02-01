#pragma once

#include "real.hpp"

#include <Furrovine++/Size2.hpp>
#include <Furrovine++/Vector2.hpp>
#include <Furrovine++/Vector3.hpp>
#include <Furrovine++/Vector4.hpp>
#include <Furrovine++/TNormal2.hpp>
#include <Furrovine++/TNormal3.hpp>
#include <Furrovine++/TNormal4.hpp>
#include <Furrovine++/TMatrix4.hpp>
#include <Furrovine++/TRectangle.hpp>
#include <Furrovine++/TSpherical.hpp>
#include <Furrovine++/Ray3.hpp>
#include <Furrovine++/optional.hpp>

namespace Fur = Furrovine;

typedef Fur::TSpherical<real> spherical;
typedef Fur::TNormal2<real> normal2;
typedef Fur::TNormal3<real> normal3;
typedef Fur::TNormal4<real> normal4;
typedef Fur::TSize2<real> size2;
typedef Fur::TSize2<std::size_t> size2u;
typedef Fur::TVector2<real> vec2;
typedef Fur::TVector2<std::size_t> vec2u;
typedef Fur::TVector2<std::ptrdiff_t> vec2i;
typedef Fur::TVector3<real> vec3;
typedef Fur::TVector3<std::size_t> vec3u;
typedef Fur::TVector4<real> vec4;
typedef Fur::TMatrix4<real> mat4;


template <typename T>
Fur::TVector3<T> reflect( const Fur::TVector3<T>& incident, const Fur::TVector3<T>& normal ) {
	const T cosi = -Fur::dot( normal, incident );
	return incident + ( static_cast<T>( 2 ) * cosi * normal );
}

template <typename T>
Fur::optional<Fur::TVector3<T>> refract( const Fur::TVector3<T>& incident, const Fur::TVector3<T>& normal, T iorn1, T iorn2 ) {
	const T iorratio = iorn1 / iorn2;
	const T cosi = -Fur::dot( normal, incident );
	const T sintsquared = ( iorratio * iorratio ) * ( static_cast<T>( 1 ) - ( cosi * cosi ) );
	if ( sintsquared > static_cast<T>( 1 ) )
		return Fur::nullopt;
	const T cost = std::sqrt( static_cast<T>( 1 ) - sintsquared );
	return ( iorratio * incident ) + ( ( ( iorratio * cosi ) - cost ) * normal );
}

template <typename T>
std::pair<T, T> fresnel( const Fur::TVector3<T>& incident, const Fur::TVector3<T>& normal, T iorn1, T iorn2 ) {
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
std::pair<T, T> shlick_fresnel( const Fur::TVector3<T>& incident, const Fur::TVector3<T>& normal, T iorn1, T iorn2 ) {
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
