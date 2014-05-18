#pragma once

#include "Output.h"
#include "RayTracer.h"
#include "Camera.h"
#include "Hitmap.h"
#include "Tile.h"
#include <Furrovine++/Threading/ThreadPool.h>

class ThreadedTileTracer {
public:
	typedef std::function<void( Fur::TVector2<std::size_t> xy, rgba color, bool multisampling )> traced_fx_t;
	typedef std::function<void( Tile tile )> tile_fx_t;

private:
	std::atomic<bool> tracecomplete;
	std::atomic<bool> multisamplecomplete;
	std::atomic<std::size_t> patches;
	std::atomic<std::size_t> multisamplepatches;
	std::atomic<std::size_t> multisampleshuffle;
	Hitmap hitmap;
	const Scene& scene;
	const Camera& camera;
	const RayTracer& raytracer;
	const RayShader& rayshader;
	Fur::optional<const Multisampler&> multisampler;
	Output& output;
	traced_fx_t ontrace;
	tile_fx_t onpretile;
	tile_fx_t ontile;

public:

	ThreadedTileTracer( std::size_t w, std::size_t h, const Camera& camera, const Scene& scene, const RayTracer& raytracer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, tile_fx_t patchfx, tile_fx_t prepatchfx, traced_fx_t tracefx )
		: tracecomplete( false ), multisamplecomplete( false ),
		patches( 0 ), multisamplepatches( 0 ), multisampleshuffle( 0 ),
		hitmap( w, h ), scene( scene ), camera( camera ), raytracer( raytracer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ),
		ontile( std::move( patchfx ) ), ontrace( std::move( tracefx ) ) {
		if ( !ontrace ) {
			ontrace = [ ] ( Fur::TVector2<std::size_t>, rgba color, bool multisampling ) -> void { };
		}
		if ( !ontile ) {
			ontile = [ ] ( Tile tile ) -> void { };
		}
		if ( !onpretile ) {
			onpretile = [ ] ( Tile tile ) -> void { };
		}
	}

	void Reset( Furrovine::Threading::ThreadPool& threadpool ) {
		auto bnd = hitmap.bounds( );
		std::size_t patchcount;
		for ( std::size_t y = 0; y < static_cast<std::size_t>( bnd[ 1 ] ); y += 16 ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( bnd[ 0 ] ); x += 16 ) {
				patchcount = patchcount + 1;
			}
		}
		patches = patchcount;
		tracecomplete = false;
		multisamplecomplete = !multisampler;
		multisamplepatches = 0;
		multisampleshuffle = 0;
		for ( std::size_t y = 0; y < static_cast<std::size_t>( bnd[ 1 ] ); y += 16 ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( bnd[ 0 ] ); x += 16 ) {
				Tile tile( Fur::TVector2<std::size_t>( x, y ), 
					Fur::TSize2<std::size_t>( std::min<std::size_t>( 16, bnd[ 0 ] - x ), std::min<std::size_t>( 16, bnd[ 0 ] - x ) ) );
				threadpool.Queue( &TracePatch, this, std::ref( threadpool ), tile );
			}
		}
	}

	bool Check( ) const {
		return tracecomplete && multisamplecomplete;
	}

	void MultisamplePatch( Furrovine::Threading::ThreadPool& threadpool, Tile tile ) {
		const Multisampler& ms = *multisampler;
		std::size_t shuffle = multisampleshuffle++;
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				rgba sample;
				for ( std::size_t s = 0; s < ms.size( ); ++s ) {
					const vec2& multisample = ms[ s + shuffle ];
					real sx = x + multisample.x;
					real sy = y + multisample.y;
					auto hit = raytracer.RayTrace( vec2( sx, sy ), hitmap.bounds( ), camera, scene, rayshader );
					sample += hit.first;
				}
				sample /= static_cast<real>( ms.size( ) );
				ontrace( { x, y }, sample, true );
				output( x, y, sample );
			}
		}
	}

	void Patch( Furrovine::Threading::ThreadPool& threadpool, Tile tile ) {
		onpretile( tile );
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				auto hit = raytracer.RayTrace( vec2( static_cast<float>( x ), static_cast<float>( y ) ), size2( hitmap.bounds( ) ), camera, scene, rayshader );
				hitmap[ { x, y } ] = hit.second.value( );
				ontrace( { x, y }, hit.first, false );
				output( x, y, hit.first );
			}
		}
		ontile( tile );

		--patches;
		tracecomplete = patches == 0;
		if ( !tracecomplete )
			return;
		
		if ( multisamplecomplete )
			return;
		
		auto bnd = hitmap.bounds( );
		for ( std::size_t y = 0; y < static_cast<std::size_t>( bnd[ 1 ] ); y += 16 ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( bnd[ 0 ] ); x += 16 ) {
				if ( !hitmap.should_multisample( { x, y } ) ) {
					continue;
				}
				Tile tile( x, y, 1, 1 );
				threadpool.Queue( &TraceMultisamplePatch, this, std::ref( threadpool ), tile );
			}
		}
	}

	static void TracePatch( ThreadedTileTracer* ptracer, std::reference_wrapper<Furrovine::Threading::ThreadPool> threadpool, Tile tile ) {
		ptracer->Patch( threadpool, tile );
	}

	static void TraceMultisamplePatch( ThreadedTileTracer* ptracer, std::reference_wrapper<Furrovine::Threading::ThreadPool> threadpool, Tile tile ) {
		ptracer->MultisamplePatch( threadpool, tile );
	}

};