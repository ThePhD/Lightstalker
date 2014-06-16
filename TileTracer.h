#pragma once

#include "Output.h"
#include "RayBouncer.h"
#include "Camera.h"
#include "Hitmap.h"
#include "Multisampler.h"
#include "Tile.h"
#include "RayTracerStep.h"
#include <Furrovine++/bounds_iterator.h>
#include <Furrovine++/Stopwatch.h>
#include <chrono>

template <std::size_t n = 16, std::size_t m = n>
class TileTracer {
private:
	Fur::bounds<2> bnd;
	std::vector<Tile> tiles;
	std::vector<Tile> multipreptiles;
	std::vector<Tile> multitiles;
	std::chrono::milliseconds timelimit;
	bool tracecomplete;
	bool multisampleprepcomplete;
	bool multisamplecomplete;
	Hitmap hitmap;
	const Scene& scene;
	const Camera& camera;
	const RayBouncer& raybouncer;
	const RayShader& rayshader;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;

	void Trace1( ) {
		Fur::Stopwatch stopwatch;
		bool timerbreak = false;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		const Multisampler& ms = *multisampler;
		real realmssize = static_cast<real>( ms.size( ) );

		stopwatch.Start( );
		while ( !multitiles.empty( ) ) {
			Tile tile = multitiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					rgba pixel{ };
					for ( std::size_t s = 0; s < ms.size( ); ++s ) {
						const vec2& multisample = ms[ s ];
						real sx = static_cast<real>(x)+multisample.x;
						real sy = static_cast<real>(y)+multisample.y;
						Ray ray = camera.Compute( sx, sy, swidth, sheight );
						auto bounce = raybouncer.Bounce( ray, scene, rayshader );
						pixel += bounce.first;
					}
					pixel /= realmssize;
					output( x, y, pixel );
				}
			}
			multitiles.pop_back( );
			timerbreak = ( stopwatch.ElapsedMilliseconds( ) > timelimit.count( ) );
			if ( timerbreak )
				return;
		}

		multisamplecomplete = true;
	}

	void Trace1Prepare( ) {
		Fur::Stopwatch stopwatch{ };

		stopwatch.Start( );
		while ( !multipreptiles.empty( ) ) {
			bool tiled = false;
			Tile tile = multipreptiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom && !tiled; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right && !tiled; ++x ) {
					if ( !hitmap.should_multisample( { x, y } ) ) {
						continue;
					}
					multitiles.push_back( tile );
					tiled = true;
				}
			}
			multipreptiles.pop_back( );
			bool timerbreak = stopwatch.ElapsedMilliseconds( ) > timelimit.count( );
			if ( timerbreak )
				return;
		}

		multisampleprepcomplete = true;
	}

	void Trace0( ) {
		Fur::Stopwatch stopwatch;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );

		stopwatch.Start( );
		while ( !tiles.empty( ) ) {
			Tile tile = tiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					Ray ray = camera.Compute( sx, sy, swidth, sheight );
					auto bounce = raybouncer.Bounce( ray, scene, rayshader );
					if ( bounce.second )
						hitmap[ { x, y } ] = bounce.second.value( );
					else
						hitmap[ { x, y } ] = 0; 
					output( x, y, bounce.first );
				}
			}
			multipreptiles.push_back( tile );
			tiles.pop_back( );
			bool timerbreak = stopwatch.ElapsedMilliseconds( ) > timelimit.count( );
			if ( timerbreak )
				return;
		}

		tracecomplete = true;
		multisampleprepcomplete = multisamplecomplete = !multisampler;
		if ( multisampleprepcomplete )
			multipreptiles.clear( );
	}

public:
	TileTracer( Fur::bounds<2> wh, const Camera& camera, const Scene& scene, const RayBouncer& raybouncer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, std::chrono::milliseconds timelimit ) : timelimit( timelimit ),
		tracecomplete( false ), multisampleprepcomplete( false ), multisamplecomplete( false ), bnd( wh ),
		hitmap( w, h ), scene( scene ), camera( camera ), raybouncer( raybouncer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ) {
		Reset( );
	}

	void Reset( ) {
		tiles.clear( );
		multipreptiles.clear( );
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
		tracecomplete = false;
		multisampleprepcomplete = !multisampler;
		multisamplecomplete = !multisampler;
	}

	bool Check( ) const {
		return tracecomplete && multisamplecomplete && multisampleprepcomplete;
	}

	RayTracerStep Steps( ) const {
		return RayTracerStep::None
			| ( !tracecomplete ? RayTracerStep::Preliminary : RayTracerStep::None )
			| ( !multisampleprepcomplete ? RayTracerStep::MultisampleDetection : RayTracerStep::None )
			| ( !multisamplecomplete ? RayTracerStep::Multisampling : RayTracerStep::None );
	}

	void Compute( ) {
		if ( Check( ) )
			return;
		if ( !tracecomplete )
			Trace0( );
		else if ( !multisampleprepcomplete )
			Trace1Prepare( );
		else if ( !multisamplecomplete )
			Trace1( );
	}

};
