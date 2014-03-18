#pragma once

#include "Primitive.h"
#include "RealRgba.h"
#include "Material.h"
#include "Light.h"
#include "RayTrace.h"
#include "RayShader.h"
#include <Furrovine++/optional.h>
#include <Furrovine++/reference_equals.h>
#include <vector>

class Scene {
private:
	Primitive vacuum;
	Material vacuummaterial;
	Hit vacuumhit;
	std::vector<Primitive> primitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;

public:

	Scene( const RealRgba& background = Fur::Colors::White ) : vacuum( vacuum_arg ), vacuummaterial( background, Fur::Colors::Transparent ) {
		vacuumhit.distance0 = vacuumhit.distance1 = std::numeric_limits<real>::max( );
		vacuumhit.normal = Vec3::Zero;
		vacuumhit.uvw = vacuumhit.contact = Vec3( std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ) );
		vacuummaterial.indexofrefraction = Ior::Vacuum;

		primitives.reserve( 9182 );
		materials.reserve( 9182 );
		ambientlights.reserve( 2 );
		pointlights.reserve( 8 );
		directionallights.reserve( 8 );
	}

	template <typename... Tn>
	void Add( Material material, Tn&&... argn ) {
		primitives.emplace_back( std::forward<Tn>( argn )... );
		materials.push_back( material );
		primitives.back( ).material = materials.size( ) - 1;
	}

	template <typename... Tn>
	void AddAmbientLight( Tn&&... argn ) {
		ambientlights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddPointLight( Tn&&... argn ) {
		pointlights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddDirectionalLight( Tn&&... argn ) {
		directionallights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddSpotLight( Tn&&... argn ) {
		//spotlights.emplace_back( std::forward<Tn>( argn )... );
	}

	Primitive& PrimitiveAt( std::size_t idx ) {
		return primitives[ idx ];
	}

	Material& MaterialOf( Primitive& primitive ) {
		return MaterialAt( primitive.material );
	}

	Material& MaterialAt( std::size_t idx ) {
		return materials[ idx ];
	}

	void Intersect( const Ray& ray, RayTrace& trace, Fur::optional<const Primitive&> ignore = Fur::nullopt ) const {
		trace.closesthit = Fur::nullopt;
		trace.hits.clear( );
		trace.orderedhits.clear( );

		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			const Primitive& prim = primitives[ p ];
			if ( ignore && Fur::reference_equals( ignore.value( ), prim ) )
				continue;
			auto hit = intersect( ray, prim );
			if ( !hit )
				continue;
			trace.hits.emplace_back( PrimitiveHit{ prim, materials[ prim.material ], hit.value( ) } );
			if ( hit->distance0 < t0 ) {
				trace.closesthit = trace.hits.back();
				t0 = hit->distance0;
			}
		}

		trace.hits.emplace_back( PrimitiveHit{ vacuum, vacuummaterial, vacuumhit } );
		if ( trace.hits.size( ) == 1 )
			trace.closesthit = trace.hits.back( );

		for ( std::size_t h = 0; h < trace.hits.size( ); ++h ) {
			trace.orderedhits.emplace_back( std::addressof( trace.hits[ h ] ) );
		}
		std::sort( trace.orderedhits.begin( ), trace.orderedhits.end( ), 
		[ ] ( PrimitiveHit* left, PrimitiveHit* right ) {
			return left->third.distance0 < right->third.distance0;
		} );
	}

	RealRgba Shading( const Ray& shadowray, const Primitive& primitive, RayShader& rayshader, RayTrace& shadowtrace ) const {
		const static RealRgba transparent = RealRgba( Fur::Colors::Transparent );
		RealRgba shadow{ static_cast<real>( 1 ), static_cast<real>( 1 ), static_cast<real>( 1 ), static_cast<real>( 1 ) };
		Intersect( shadowray, shadowtrace, primitive );
		if ( !shadowtrace.closesthit || shadowtrace.closesthit->first.id == PrimitiveId::Vacuum )
			return shadow;
		for ( std::size_t s = 0; s < shadowtrace.orderedhits.size( ); ++s ) {
			PrimitiveHit& shadowphit = *shadowtrace.orderedhits[ s ];
			const Primitive& shadowprimitive = shadowphit.first;
			const Material& shadowmaterial = shadowphit.second;
			Hit& shadowhit = shadowphit.third;
			if ( shadowmaterial.transparency.length_squared( ) == static_cast<real>( 0 ) ) {
				shadow = { 0, 0, 0, 0 };
				break;
			}
			shadow -= shadowmaterial.transparency;
		}
		shadow.max( Vec4::Zero );
		return shadow;
	}

	RealRgba Lighting( const Ray& ray, RayShader& rayshader, PrimitiveHit& primitivehit, RayTrace& shadowtrace ) const {
		using namespace Fur::Colors;

		real perambient = static_cast<real>( 1 ) / ambientlights.size( );
		real perdirectional = static_cast<real>( 1 ) / directionallights.size( );
		real perpoint = static_cast<real>( 1 ) / pointlights.size( );

		RealRgba color{ };
		
		RealRgba ambient{ };
		RealRgba directional{ };
		RealRgba point{ };
		
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		Hit& hit = primitivehit.third;
		const Vec3& surfacecontact = hit.contact;
		
		for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
			ambient += rayshader( ray, primitivehit, ambientlights[ a ] ) * perambient;
		}

		for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
			Ray shadowray( surfacecontact, -directionallights[ d ].direction );
			RealRgba shadow = Shading( shadowray, primitive, rayshader, shadowtrace );
			directional += shadow * rayshader( ray, primitivehit, directionallights[ d ] ) * perdirectional;
		}

		for ( std::size_t p = 0; p < pointlights.size( ); ++p ) {
			Vec3 dir = surfacecontact.direction_to( pointlights[ p ].position );
			Ray shadowray( surfacecontact, dir );
			RealRgba shadow = Shading( shadowray, primitive, rayshader, shadowtrace );
			point += shadow * rayshader( ray, primitivehit, pointlights[ p ] ) * perpoint;
		}

		color += ambient + point + directional;
		return color;
	}

};
