#pragma once

#include "real.h"
#include <vector>
#include <Furrovine++/Sampling.h>

class Multisampler {
private:
	std::size_t n;
	std::size_t m;
	std::vector<Vec2> multisamples;
	std::size_t shuffle;

public:
	template <typename TGen>
	Multisampler( std::size_t nx, std::size_t my, TGen&& gen ) : n( nx ), m( my ), multisamples( Fur::Sampling::jittered<real>( n, m, std::forward<TGen>( gen ) ) ) {

	}

	std::size_t size( ) const {
		return multisamples.size( );
	}

	void Shuffle( ) {
		shuffle = shuffle + 1 % multisamples.size( );
	}

	const Vec2& operator[] ( std::size_t idx ) const {
		return multisamples[ ( idx + shuffle ) % multisamples.size( ) ];
	}

};