#pragma once

#include "Output.h"
#include "RayTracer.h"
#include "Camera.h"
#include "Hitmap.h"
#include "Multisampler.h"
#include <Furrovine++/Stopwatch.h>
#include <chrono>

class TileTracer {
private:
	std::size_t x, y;
	std::chrono::milliseconds timelimit;
	bool tracecomplete;
	bool multisamplecomplete;
	Hitmap hitmap;
	const Scene& scene;
	const Camera& camera;
	const RayTracer& raytracer;
	const RayShader& rayshader;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;

	void Trace0( std::chrono::milliseconds timelimit ) {
		Fur::Stopwatch stopwatch;
		stopwatch.Start( );
		bool timerbreak = false;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		const Multisampler& ms = *multisampler;
		std::size_t shuffle = 0;

		for ( ; y < height && !timerbreak; ) {
			for ( ; x < width && !timerbreak; ++x ) {
				real sx = static_cast<real>( x );
				real sy = static_cast<real>( y );
				Ray ray = camera.Compute( sx, sy, swidth, sheight );
				auto bounce = raytracer.Bounce( ray, scene, rayshader );
				if ( bounce.second ) 
					hitmap[ { x, y } ] = std::addressof( bounce.second.value() );
				timerbreak = ( stopwatch.ElapsedMilliseconds( ) > timelimit.count( ) );
				output( x, y, bounce.first );
			}
			if ( x == width ) {
				++y;
				x = 0;
			}
		}
		tracecomplete = x == width && y == height;
		if ( !tracecomplete )
			return;
		x = 0;
		y = 0;
		multisamplecomplete = !multisampler;
	}

	void Trace1( std::chrono::milliseconds timelimit ) {
		Fur::Stopwatch stopwatch;
		stopwatch.Start( );
		bool timerbreak = false;
		std::size_t width = hitmap.bounds( )[ 0 ];
		std::size_t height = hitmap.bounds( )[ 1 ];
		real swidth = static_cast<real>( width );
		real sheight = static_cast<real>( height );
		const Multisampler& ms = *multisampler;
		std::size_t shuffle = 0;

		for ( ; y < height && !timerbreak; ) {
			for ( ; x < width && !timerbreak; ++x ) {
				if ( !hitmap.should_multisample( { x, y } ) )
					continue;
				rgba pixel{ };
				for ( std::size_t s = 0; s < ms.size( ); ++s ) {
					const vec2& multisample = ms[ s + shuffle ];
					real sx = x + multisample.x;
					real sy = y + multisample.y;
					Ray ray = camera.Compute( sx, sy, swidth, sheight );
					auto bounce = raytracer.Bounce( ray, scene, rayshader );
					pixel += bounce.first;
				}
				timerbreak = ( stopwatch.ElapsedMilliseconds( ) > timelimit.count( ) );
				pixel /= static_cast<real>( ms.size( ) );
				output( x, y, pixel );
			}
			if ( x == width ) {
				++y;
				x = 0;
			}
		}
		multisamplecomplete = x == width && y == height;
	}

public:
	TileTracer( std::size_t w, std::size_t h, const Camera& camera, const Scene& scene, const RayTracer& raytracer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, std::chrono::milliseconds timelimit )
		: x(0), y(0), timelimit( timelimit ), tracecomplete(false), multisamplecomplete(false), hitmap( w, h ), scene( scene ), camera( camera ), raytracer( raytracer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ) {
		
	}

	void Reset( ) {
		x = 0;
		y = 0;
		tracecomplete = false;
		multisamplecomplete = false;
	}

	bool Check( ) const {
		return tracecomplete && multisamplecomplete;
	}

	void Compute( ) {
		if ( tracecomplete && multisamplecomplete )
			return;
		if ( !tracecomplete )
			Trace0( timelimit );
		else if ( !multisamplecomplete )
			Trace1( timelimit );
	}
};
