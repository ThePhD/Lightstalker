#pragma once

#include "Output.hpp"
#include "RayBouncer.hpp"
#include "Camera.hpp"
#include "Hitmap.hpp"
#include "Tile.hpp"
#include "RayTracerStep.hpp"
#include <Furrovine++/scoped_destructor.hpp>
#include <Furrovine++/Threading/ThreadPool.hpp>

template <std::size_t n = 16, std::size_t m = n>
class ThreadedTileTracer {
public:
	typedef std::function<void( vec2u xy, RayBounce& color, bool multisampling )> traced_fx_t;
	typedef std::function<void( Tile tile, bool multisampling )> tile_fx_t;

private:
	std::atomic<std::size_t> patches, multisamplepreppatches, multisamplepatches;
	std::atomic<bool> complete, multisamplecomplete, multisampleprepcomplete, stopping;
	std::mutex stopmutex;
	std::condition_variable stopcv;
	Hitmap hitmap;
	const vec2u& imagesize;
	const Scene& scene;
	const Camera& camera;
	const RayBouncer& raybouncer;
	const RayShader& rayshader;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;
	Furrovine::Threading::ThreadPool& threadpool;
	traced_fx_t ontrace;
	tile_fx_t onpretile;
	tile_fx_t ontile;

	void Patch( Tile tile ) {
		auto dx = Fur::make_destructor( [ & ] {
			--patches;
			complete = patches == 0;
			if ( !complete )
				return;
			if ( stopping ) {
				multisampleprepcomplete = true;
				multisamplecomplete = true;
				stopcv.notify_one( );
				return;
			}
			if ( multisampleprepcomplete )
				return;
			hitmap.prepare_multisample( );
			for ( std::size_t y = 0; y < static_cast<std::size_t>( imagesize[ 1 ] ); y += m ) {
				for ( std::size_t x = 0; x < static_cast<std::size_t>( imagesize[ 0 ] ); x += n ) {
					if ( stopping )
						return;
					vec2u pos( x, y );
					size2u siz( std::min<std::size_t>( n, imagesize[ 0 ] - x ), std::min<std::size_t>( m, imagesize[ 1 ] - y ) );
					Tile patchtile( pos, siz );
					threadpool.Queue( &ThreadedTileTracer<n, m>::MultisamplePreparePatch, std::ref( *this ), patchtile );
				}
			}
		} );
		onpretile( tile, false );
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				if ( stopping )
					return;
				auto bounce = raybouncer.Bounce( vec2( static_cast<float>( x ), static_cast<float>( y ) ), size2( hitmap.bounds( ).values( ) ), camera, scene, rayshader );
				hitmap[ { x, y } ] = bounce.hitid;
				ontrace( { x, y }, bounce, false );
				output( x, y, bounce );
			}
		}
		ontile( tile, false );
	}

	void MultisamplePreparePatch( Tile tile ) {
		auto dx = Fur::make_destructor( [ & ] {
			--multisamplepreppatches;
			multisampleprepcomplete = multisamplepreppatches == 0;
			if ( !multisampleprepcomplete )
				return;
			if ( stopping && multisamplecomplete ) {
				stopcv.notify_one( );
			}
		} );
		bool sampled = false;
		for ( std::size_t y = tile.top; y < tile.bottom && !sampled; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right && !sampled; ++x ) {
				if ( stopping )
					return;
				if ( !hitmap.should_multisample( { x, y } ) ) {
					continue;
				}
				multisamplecomplete = false;
				++multisamplepatches;
				threadpool.Queue( &ThreadedTileTracer<n, m>::MultisamplePatch, std::ref( *this ), tile );
				sampled = true;
			}
		}
	}

	void MultisamplePatch( Tile tile ) {
		auto dx = Fur::make_destructor( [ & ] ( ) {
			--multisamplepatches;
			multisamplecomplete = multisamplepatches == 0;
			if ( !multisamplecomplete )
				return;
			// Signal if we are stopping and the MultisamplePreparePatch
			// has already finished (otherwise, MultisamplePreparePatch will do it for us)
			if ( stopping && multisampleprepcomplete ) {
				stopcv.notify_one( );
			}
		} );

		const Multisampler& ms = *multisampler;
		real realmssize = static_cast<real>( ms.size( ) );
		vec2 swh = imagesize;

		onpretile( tile, true );
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				vec2 xy( static_cast<real>( x ), static_cast<real>( y ) );
				RayBounce pixelbounce{ };
				for ( std::size_t s = 0; s < ms.size( ); ++s ) {
					if ( stopping )
						return;
					vec2 sxy = ms[ s ];
					sxy += xy;
					Ray ray = camera.Compute( sxy, swh );
					auto bounce = raybouncer.Bounce( ray, scene, rayshader );
					pixelbounce.accumulate( bounce );
				}
				pixelbounce.color /= realmssize;
				ontrace( { x, y }, pixelbounce, true );
				output( x, y, pixelbounce );
			}
		}
		ontile( tile, true );
	}

public:

	ThreadedTileTracer( Furrovine::Threading::ThreadPool& pool, const vec2u& imagesize, const Camera& camera, const Scene& scene, const RayBouncer& raybouncer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, tile_fx_t patchfx = nullptr, tile_fx_t prepatchfx = nullptr, traced_fx_t tracefx = nullptr )
		: complete( true ), multisampleprepcomplete( true ), multisamplecomplete( true ), stopping( false ),
		imagesize( imagesize ), hitmap( imagesize ), 
		scene( scene ), camera( camera ), raybouncer( raybouncer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ),
		threadpool( pool ),
		ontile( std::move( patchfx ) ), onpretile( std::move( onpretile ) ), ontrace( std::move( tracefx ) ) {
		if ( !ontrace ) {
			ontrace = [ ] ( vec2u, RayBounce&, bool ) -> void { };
		}
		if ( !ontile ) {
			ontile = [ ] ( Tile, bool ) -> void { };
		}
		if ( !onpretile ) {
			onpretile = [ ] ( Tile, bool ) -> void { };
		}
		Reset( );
	}

	void Stop( ) {
		std::unique_lock<std::mutex> stoplock( stopmutex );
		auto pred = [ & ] ( ) -> bool { 
			return Check( ); 
		};
		stopping = true;
		stopcv.wait( stoplock, pred );
		stopping = false;
	}

	void Reset( ) {
		Stop( );
		std::size_t patchcount = 0;
		for ( std::size_t y = 0; y < static_cast<std::size_t>( imagesize[ 1 ] ); y += n ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( imagesize[ 0 ] ); x += m ) {
				++patchcount;
			}
		}
		
		patches = patchcount;
		multisamplepreppatches = patchcount;
		multisamplepatches = 0;
		complete = false;
		multisamplecomplete = true;
		multisampleprepcomplete = !multisampler || multisampler->size( ) < 2;
		
		for ( std::size_t y = 0; y < static_cast<std::size_t>( imagesize[ 1 ] ); y += m ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( imagesize[ 0 ] ); x += n ) {
				vec2u pos( x, y );
				size2u siz( std::min<std::size_t>( n, imagesize[ 0 ] - x ), std::min<std::size_t>( m, imagesize[ 1 ] - y ) );
				Tile tile( pos, siz );
				threadpool.Queue( &ThreadedTileTracer<n, m>::Patch, std::ref( *this ), tile );
			}
		}
	}

	bool Check( ) const {
		return complete && multisampleprepcomplete && multisamplecomplete;
	}

	RayTracerStep Steps( ) const {
		return RayTracerStep::None
			| ( !complete ? RayTracerStep::Preliminary : RayTracerStep::None )
			| ( !multisampleprepcomplete ? RayTracerStep::MultisampleDetection : RayTracerStep::None )
			| ( !multisamplecomplete ? RayTracerStep::Multisampling : RayTracerStep::None );
	}

	void Compute( ) {

	}

	~ThreadedTileTracer( ) {
		Stop( );
	}
};