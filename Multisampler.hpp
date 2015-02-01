#pragma once

#include "real.hpp"
#include <Furrovine++/Sampling.hpp>
#include <vector>

class Multisampler {
private:
	std::size_t n, m;
	std::vector<vec2> multisamples;

public:
	template <typename TGen>
	Multisampler( std::size_t nx, std::size_t my, TGen&& gen ) 
	: n( nx ), m( my ), multisamples( Fur::Sampling::jittered<real>( n, m, std::forward<TGen>( gen ) ) ) {

	}

	std::size_t size( ) const {
		return multisamples.size( );
	}

	const vec2& operator[] ( std::size_t idx ) const {
		return multisamples[ idx % multisamples.size( ) ];
	}

};