#pragma once

#include "real.h"
#include "Primitive.h"
#include <vector>
#include <Furrovine++/buffer_view.h>

class Hitmap {
private:
	std::size_t w, h;
	std::vector<Primitive const*> primitives;
	Fur::buffer_view<Primitive const*, 2> hitmap;
	typedef Fur::buffer_view<Primitive const*, 2>::index_type idx_t;
	typedef Fur::buffer_view<Primitive const*, 2>::bounds_type bounds_t;

public:

	Hitmap( std::size_t wx, std::size_t hy ) : w( wx ), h( hy ), primitives( w * h ), hitmap( primitives.data(), { w, h } ) {
		
	}

	std::size_t size( ) const {
		return primitives.size( );
	}

	bounds_t bounds( ) const {
		return hitmap.bounds( );
	}

	bool should_multisample( idx_t idx ) const {
		Primitive const* primitive = hitmap[ idx ];
		// Left Side
		if ( idx[ 0 ] > 0 ) {
			if ( primitive != hitmap[ idx - idx_t{ 1, 0 } ] ) {
				return true;
			}
		}
		// Top Side
		if ( idx[ 1 ] > 0 ) {
			if ( primitive != hitmap[ idx - idx_t{ 0, 1 } ] ) {
				return true;
			}
		}
		// Right Side
		if ( idx[ 0 ] < static_cast<ptrdiff_t>( w ) ) {
			if ( primitive != hitmap[ idx + idx_t{ 1, 0 } ] ) {
				return true;
			}
		}
		// Bottom Side
		if ( idx[ 1 ] < static_cast<ptrdiff_t>( h - 1 ) ) {
			if ( primitive != hitmap[ idx + idx_t{ 0, 1 } ] ) {
				return true;
			}
		}
		return false;
	}

	Primitive const* const& operator[] ( idx_t idx ) const {
		return hitmap[ idx ];
	}

	Primitive const*& operator[] ( idx_t idx ) {
		return hitmap[ idx ];
	}

};