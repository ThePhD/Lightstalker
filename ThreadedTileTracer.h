#pragma once

#include "Output.h"
#include "RayBouncer.h"
#include "Camera.h"
#include "Hitmap.h"
#include "Tile.h"
#include "RayTracerStep.h"
#include <Furrovine++/Threading/ThreadPool.h>

template <std::size_t n = 16, std::size_t m = n>
class ThreadedTileTracer {
public:
	typedef std::function<void( Fur::TVector2<std::size_t> xy, rgba color, bool multisampling )> traced_fx_t;
	typedef std::function<void( Tile tile, bool multisampling )> tile_fx_t;

private:
	Fur::bounds<2> bnd;
	std::atomic<std::size_t> patches, multisamplepatches;
	std::atomic<bool> tracecomplete;
	std::atomic<bool> multisamplecomplete;
	std::atomic<bool> multisampleprepcomplete;
	Hitmap hitmap;
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
		onpretile( tile, false );
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				auto bounce = raybouncer.RayBounce( vec2( static_cast<float>( x ), static_cast<float>( y ) ), size2( hitmap.bounds( ) ), camera, scene, rayshader );
				if ( bounce.second )
					hitmap[ { x, y } ] = bounce.second.value( );
				else
					hitmap[ { x, y } ] = 0;
				ontrace( { x, y }, bounce.first, false );
				output( x, y, bounce.first );
			}
		}
		ontile( tile, false );

		--patches;
		tracecomplete = patches == 0;
		if ( !tracecomplete )
			return;

		if ( multisamplecomplete )
			return;

		threadpool.Queue( &ThreadedTileTracer<n, m>::MultisamplePreparePatch, std::ref( *this ) );
	}

	void MultisamplePreparePatch( ) {
		for ( std::size_t my = 0; my < static_cast<std::size_t>( bnd[ 1 ] ); my += m ) {
			for ( std::size_t nx = 0; nx < static_cast<std::size_t>( bnd[ 0 ] ); nx += n ) {
				bool tiled = false;
				std::size_t sizem = std::min<std::size_t>( m, bnd[ 1 ] - my );
				std::size_t sizen = std::min<std::size_t>( n, bnd[ 0 ] - nx );
				std::size_t mlimit = my + sizem;
				std::size_t nlimit = nx + sizen;
				for ( std::size_t y = my; y < mlimit && !tiled; ++y ) {
					for ( std::size_t x = nx; x < nlimit && !tiled; ++x ) {
						if ( !hitmap.should_multisample( { x, y } ) ) {
							continue;
						}
						Fur::TVector2<std::size_t> pos( nx, my );
						Fur::TSize2<std::size_t> siz( sizen, sizem );
						Tile tile( pos, siz );
						++multisamplepatches;
						threadpool.Queue( &ThreadedTileTracer<n, m>::MultisamplePatch, std::ref( *this ), tile );
						tiled = true;
					}
				}
			}
		}

		multisampleprepcomplete = true;
	}

	void MultisamplePatch( Tile tile ) {
		const Multisampler& ms = *multisampler;
		real realmssize = static_cast<real>( ms.size( ) );
		onpretile( tile, true );
		for ( std::size_t y = tile.top; y < tile.bottom; ++y ) {
			for ( std::size_t x = tile.left; x < tile.right; ++x ) {
				rgba sample;
				for ( std::size_t s = 0; s < ms.size( ); ++s ) {
					const vec2& multisample = ms[ s ];
					real sx = x + multisample.x;
					real sy = y + multisample.y;
					vec2 sv( sx, sy );
					auto bounce = raybouncer.RayBounce( sv, hitmap.bounds( ), camera, scene, rayshader );
					sample += bounce.first;
				}
				sample /= realmssize;
				ontrace( { x, y }, sample, true );
				output( x, y, sample );
			}
		}
		ontile( tile, true );
		--multisamplepatches;
		multisamplecomplete = multisamplepatches == 0;
	}

public:

	ThreadedTileTracer( Furrovine::Threading::ThreadPool& pool, Fur::bounds<2> wh, const Camera& camera, const Scene& scene, const RayBouncer& raybouncer, const RayShader& rayshader, Fur::optional<const Multisampler&> multisampler, Output& output, tile_fx_t patchfx = nullptr, tile_fx_t prepatchfx = nullptr, traced_fx_t tracefx = nullptr )
		: tracecomplete( false ), multisamplecomplete( false ), bnd( wh ),
		hitmap( wh, reinterpret_cast<uintptr_t>( std::addressof( scene.Vacuum().first ) ) ), scene( scene ), camera( camera ), raybouncer( raybouncer ),
		rayshader( rayshader ), multisampler( std::move( multisampler ) ), output( output ),
		threadpool( pool ),
		ontile( std::move( patchfx ) ), onpretile( std::move( onpretile ) ), ontrace( std::move( tracefx ) ) {
		if ( !ontrace ) {
			ontrace = [ ] ( Fur::TVector2<std::size_t>, rgba, bool ) -> void { };
		}
		if ( !ontile ) {
			ontile = [ ] ( Tile tile, bool multisampling ) -> void { };
		}
		if ( !onpretile ) {
			onpretile = [ ] ( Tile tile, bool multisampling ) -> void { };
		}
		Reset( );
	}

	void Reset( ) {
		std::size_t patchcount = 0;
		for ( std::size_t y = 0; y < static_cast<std::size_t>( bnd[ 1 ] ); y += n ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( bnd[ 0 ] ); x += m ) {
				++patchcount;
			}
		}
		
		patches = patchcount;
		multisamplepatches = 0;
		tracecomplete = false;
		multisamplecomplete = !multisampler;
		multisampleprepcomplete = !multisampler;
		
		for ( std::size_t y = 0; y < static_cast<std::size_t>( bnd[ 1 ] ); y += m ) {
			for ( std::size_t x = 0; x < static_cast<std::size_t>( bnd[ 0 ] ); x += n ) {
				Fur::TVector2<std::size_t> pos( x, y );
				Fur::TSize2<std::size_t> siz( std::min<std::size_t>( n, bnd[ 0 ] - x ), std::min<std::size_t>( m, bnd[ 1 ] - y ) );
				Tile tile( pos, siz );
				threadpool.Queue( &ThreadedTileTracer<n, m>::Patch, std::ref( *this ), tile );
			}
		}
	}

	bool Check( ) const {
		return tracecomplete && multisampleprepcomplete && multisamplecomplete;
	}

	RayTracerStep Steps( ) const {
		return RayTracerStep::None
			| ( !tracecomplete ? RayTracerStep::Preliminary : RayTracerStep::None )
			| ( !multisampleprepcomplete ? RayTracerStep::MultisampleDetection : RayTracerStep::None )
			| ( !multisamplecomplete ? RayTracerStep::Multisampling : RayTracerStep::None );
	}

	void Compute( ) {

	}

};