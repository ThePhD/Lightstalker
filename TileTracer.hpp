#pragma once

#include "Output.hpp"
#include "RayBouncer.hpp"
#include "Camera.hpp"
#include "Hitmap.hpp"
#include "Multisampler.hpp"
#include "Tile.hpp"
#include "RayTracerStep.hpp"
#include <Furrovine++/bounds_iterator.hpp>
#include <Furrovine++/Stopwatch.hpp>
#include <chrono>

template <std::size_t n = 16, std::size_t m = n>
class TileTracer {
private:
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
	const vec2u& imagesize;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;

	void Trace1( ) {
		Fur::stopwatch<> stopwatch;
		const Multisampler& ms = *multisampler;
		bool timerbreak = false;
		vec2 swh = imagesize;
		real realmssize = static_cast<real>( ms.size( ) );

		stopwatch.start( );
		while ( !multitiles.empty( ) ) {
			Tile tile = multitiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					vec2 xy( static_cast<real>( x ), static_cast<real>( y ) );
					RayBounce pixelbounce{ };
					for ( std::size_t s = 0; s < ms.size( ); ++s ) {
						vec2 sxy = ms[ s ];
						sxy += xy;
						Ray ray = camera.Compute( sxy, swh );
						auto bounce = raybouncer.Bounce( ray, scene, rayshader );
						pixelbounce.accumulate( bounce );
					}
					pixelbounce.color /= realmssize;
					output( x, y, pixelbounce );
				}
			}
			multitiles.pop_back( );
			timerbreak = ( stopwatch.elapsed( ) > timelimit );
			if ( timerbreak )
				return;
		}

		multisamplecomplete = true;
	}

	void Trace1Prepare( ) {
		Fur::stopwatch<> stopwatch;

		stopwatch.start( );
		while ( !multipreptiles.empty( ) ) {
			bool tiled = false;
			Tile tile = multipreptiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom && !tiled; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right && !tiled; ++x ) {
					if ( !hitmap.should_multisample( { x, y } ) ) {
						continue;
					}
					multisamplecomplete = false;
					multitiles.push_back( tile );
					tiled = true;
				}
			}
			multipreptiles.pop_back( );
			bool timerbreak = stopwatch.elapsed( ) > timelimit;
			if ( timerbreak )
				return;
		}

		multisampleprepcomplete = true;
	}

	void Trace0( ) {
		Fur::stopwatch<> stopwatch;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );

		stopwatch.start( );
		while ( !tiles.empty( ) ) {
			Tile tile = tiles.back( );
			for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
				for ( std::size_t x = tile.left; x < tile.right; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					Ray ray = camera.Compute( sx, sy, swidth, sheight );
					auto bounce = raybouncer.Bounce( ray, scene, rayshader );
					hitmap[ { x, y } ] = bounce.hitid;
					output( x, y, bounce );
				}
			}
			multipreptiles.push_back( tile );
			tiles.pop_back( );
			bool timerbreak = stopwatch.elapsed( ) > timelimit;
			if ( timerbreak )
				return;
		}
		hitmap.prepare_multisample( );
		tracecomplete = true;
		multisampleprepcomplete = multisamplecomplete = !multisampler || multisampler->size( ) < 2;
		if ( multisampleprepcomplete )
			multipreptiles.clear( );
	}

public:
	TileTracer( const vec2u& imagesize, const Camera& camera, const Scene& scene, const RayBouncer& raybouncer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, std::chrono::milliseconds timelimit ) : timelimit( timelimit ),
	tracecomplete( false ), multisampleprepcomplete( false ), multisamplecomplete( false ), 
	imagesize( imagesize ), hitmap( imagesize ), 
	scene( scene ), camera( camera ), raybouncer( raybouncer ),
	rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ) {
		Reset( );
	}

	void Stop( ) {
		tiles.clear( );
		multipreptiles.clear( );
		multitiles.clear( );
		Compute( );
	}

	void Reset( ) {
		Stop( );

		std::size_t width = imagesize[ 0 ];
		std::size_t height = imagesize[ 1 ];
		
		for ( std::size_t y = 0; y < height; y += m ) {
			for ( std::size_t x = 0; x < width; x += n ) {
				Tile tile( x, y, x + n < width ? n : width - x, y + m < height ? m : height - y );
				tiles.push_back( tile );
			}
		}
		tracecomplete = false;
		multisampleprepcomplete = !multisampler || multisampler->size( ) < 2;
		multisamplecomplete = true;
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
