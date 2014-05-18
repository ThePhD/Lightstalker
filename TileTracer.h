#pragma once

#include "Output.h"
#include "RayTracer.h"
#include "Camera.h"
#include "Hitmap.h"
#include "Multisampler.h"
#include "Tile.h"
#include <Furrovine++/bounds_iterator.h>
#include <Furrovine++/Stopwatch.h>
#include <chrono>

template <std::size_t n = 16, std::size_t m = n>
class TileTracer {
private:
	Fur::bounds_iterator<2> mtit;
	std::vector<Tile> tiles;
	std::vector<Tile> multitiles;
	std::chrono::milliseconds timelimit;
	bool tracecomplete;
	bool multisampleprepcomplete;
	bool multisamplecomplete;
	Hitmap hitmap;
	const Scene& scene;
	const Camera& camera;
	const RayTracer& raytracer;
	const RayShader& rayshader;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;

	void Trace0( ) {
		Fur::Stopwatch stopwatch;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		
		stopwatch.Start( );
		while ( !tiles.empty() ) {
			Tile tile = tiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					Ray ray = camera.Compute( sx, sy, swidth, sheight );
					auto bounce = raytracer.Bounce( ray, scene, rayshader );
					if ( bounce.second )
						hitmap[ { x, y } ] = bounce.second.value( );
					output( x, y, bounce.first );
				}
			}
			tiles.pop_back( );
			bool timerbreak = stopwatch.ElapsedMilliseconds( ) > timelimit.count( );
			if ( timerbreak )
				return;
		}
		
		tracecomplete = true;
		multisampleprepcomplete = multisamplecomplete = !multisampler;
	}

	void Trace1Prepare( ) {
		Fur::Stopwatch stopwatch;
		auto mtend = Fur::end( hitmap.bounds( ) );
		bool timerbreak = false;
		
		stopwatch.Start( );
		while ( mtit != mtend ) {
			auto midx = *mtit++;
			if ( hitmap.should_multisample( midx ) ) {
				multitiles.push_back( Tile( midx[ 0 ], midx[ 1 ], 1, 1 ) );
			}
			timerbreak = stopwatch.ElapsedMilliseconds( ) > timelimit.count( );
			if ( timerbreak )
				return;
		}
		
		multisampleprepcomplete = true;
	}

	void Trace1( ) {
		Fur::Stopwatch stopwatch;
		bool timerbreak = false;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		const Multisampler& ms = *multisampler;
		std::size_t shuffle = 0;

		stopwatch.Start( );
		while ( !multitiles.empty( ) ) {
			Tile tile = multitiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					rgba pixel{ };
					for ( std::size_t s = 0; s < ms.size( ); ++s ) {
						const vec2& multisample = ms[ s + shuffle ];
						real sx = static_cast<real>( x ) + multisample.x;
						real sy = static_cast<real>( y ) + multisample.y;
						Ray ray = camera.Compute( sx, sy, swidth, sheight );
						auto bounce = raytracer.Bounce( ray, scene, rayshader );
						pixel += bounce.first;
					}
					pixel /= static_cast<real>( ms.size( ) );
					output( x, y, pixel );
				}
			}
			multitiles.pop_back( );
			timerbreak = ( stopwatch.ElapsedMilliseconds( ) > timelimit.count( ) );
			if ( timerbreak )
				return;
		}

		multisampleprepcomplete = true;
	}

public:
	TileTracer( std::size_t w, std::size_t h, const Camera& camera, const Scene& scene, const RayTracer& raytracer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, std::chrono::milliseconds timelimit )
		: timelimit( timelimit ), tracecomplete(false), multisampleprepcomplete(false), multisamplecomplete(false), hitmap( w, h ), scene( scene ), camera( camera ), raytracer( raytracer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ) {
		Reset( );
	}

	void Reset( ) {
		tiles.clear( );
		multitiles.clear( );
		
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		
		for ( std::size_t y = 0; y < height; y += m ) {
			for ( std::size_t x = 0; x < width; x += n ) {
				Tile tile( x, y, x + n < width ? n : width - x, y + m < height ? m : height - y );
				tiles.push_back( tile );
			}
		}
		mtit = Fur::begin( hitmap.bounds( ) );
		tracecomplete = false;
		multisampleprepcomplete = false;
		multisamplecomplete = false;
	}

	bool Check( ) const {
		return tracecomplete && multisamplecomplete && multisampleprepcomplete;
	}

	void Compute( ) {
		if ( Check() )
			return;
		if ( !tracecomplete )
			Trace0( );
		else if ( !multisampleprepcomplete )
			Trace1Prepare( );
		else if ( !multisamplecomplete )
			Trace1( );
	}
};
