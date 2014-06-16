#pragma once

#include "real.h"
#include "Primitive.h"
#include <vector>
#include <Furrovine++/buffer_view.h>

class Hitmap {
private:
	typedef Fur::buffer_view<uintptr_t, 2>::index_type idx_t;
	typedef Fur::buffer_view<uintptr_t, 2>::bounds_type bounds_t;
	static const idx_t skew;
	static const idx_t to_left;
	static const idx_t to_right;
	static const idx_t to_up;
	static const idx_t to_down;
	bounds_t visiblebnd;
	bounds_t bnd;
	std::vector<uintptr_t> primitives;
	Fur::buffer_view<uintptr_t, 2> hitmap;
	
public:

	Hitmap( const bounds_t& wh, uintptr_t defaultvalue = 0 ) : 
	visiblebnd( wh ), bnd( wh + bounds_t(2, 2) ), 
	primitives( bnd[0] * bnd[1], defaultvalue ), 
	hitmap( primitives.data(), bnd ) {
		
	}

	std::size_t size( ) const {
		return primitives.size( );
	}

	bounds_t bounds( ) const {
		return visiblebnd;
	}

	bool should_multisample( idx_t idx ) const {
		idx += skew;
		const uintptr_t& primitive = hitmap[ idx ];
		
		// Left Side
		idx_t left = idx + to_left;
		if ( primitive != hitmap[ left ] ) {
			return true;
		}
		
		// Top Side
		idx_t top = idx + to_up;
		if ( primitive != hitmap[ top ] ) {
			return true;
		}

		// Right Side
		idx_t right = idx + to_right;
		if ( primitive != hitmap[ right ] ) {
			return true;
		}
		
		// Bottom Side
		idx_t bottom = idx + to_down;
		if ( primitive != hitmap[ bottom ] ) {
			return true;
		}

		return false;
	}

	const uintptr_t& operator[] ( idx_t idx ) const {
		idx += skew;
		return hitmap[ idx ];
	}

	uintptr_t& operator[] ( idx_t idx ) {
		idx += skew;
		return hitmap[ idx ];
	}

};

const Hitmap::idx_t Hitmap::skew = idx_t{ 1, 1 };
const Hitmap::idx_t Hitmap::to_left = Hitmap::idx_t{ -1, 0 };
const Hitmap::idx_t Hitmap::to_right = Hitmap::idx_t{ 1, 0 };
const Hitmap::idx_t Hitmap::to_up = Hitmap::idx_t{ 0, 1 };
const Hitmap::idx_t Hitmap::to_down = Hitmap::idx_t{ 0, -1 };
