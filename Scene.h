#include "Primitive.h"
#include "RealRgba.h"
#include "Material.h"
#include "Light.h"
#include "Trace.h"
#include "RayShader.h"
#include <Furrovine++/optional.h>
#include <Furrovine++/reference_equals.h>
#include <vector>

class Scene {
private:
	std::vector<Primitive> primitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;

public:

	Scene( ) {
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

	void Intersect( const Ray& ray, Trace& trace, Fur::optional<const Primitive&> ignore = Fur::nullopt ) {
		trace.closesthit = Fur::nullopt;
		trace.hits.clear( );

		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			Primitive& prim = primitives[ p ];
			if ( ignore && Fur::reference_equals( ignore.value( ), prim ) )
				continue;
			auto hit = intersect( ray, prim );
			if ( !hit )
				continue;
			trace.hits.emplace_back( PrimitiveHit{ prim, materials[ prim.material ], hit.value( ) } );
			if ( hit->distance0 < t0 ) {
				trace.closesthit = PrimitiveHit{ prim, materials[ prim.material ], hit.value( ) };
				t0 = hit->distance0;
			}
		}
	}

	std::pair<RealRgba, RealRgba> Shading( const Ray& ray, RayShader& rayshader, Trace& trace ) {
		/*Intersect( shadowray, shadowtrace, primitive );
		if ( !shadowtrace.closesthit )
			return false;

		Primitive& shadowprimitive = shadowtrace.closesthit->first;
		Material& shadowmaterial = materials[ shadowprimitive.material ];
		if ( length_squared( shadowmaterial.transmission ) == static_cast<real>( 0 ) ) {
			// No transmission: full black
			color = Black;
		}

		RealRgba shadowinfluence = lerp_components( RealRgba( Black ),
			shadowmaterial.diffuse,
			shadowmaterial.transmission );
		color += shadowinfluence;*/
		return{ };
	}

	RealRgba Lighting( const Ray& ray, RayShader& rayshader, Trace& trace, Trace& shadowtrace ) {
		using namespace Fur::Colors;

		if ( !trace.closesthit )
			return RealRgba{ };
		
		RealRgba color{ };
		RealRgba shadowcolor{ };
		RealRgba shadowpower{ };
		RealRgba ambient{ };
		RealRgba directional{ };
		RealRgba point{ };

		Primitive& primitive = trace.closesthit->first;
		Material& material = trace.closesthit->second;
		Hit& hit = trace.closesthit->third;
		const Vec3& surfacecontact = hit.contact;
		
		for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
			ambient += rayshader( ray, trace, ambientlights[ a ] );
		}
		for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
			Ray shadowray( surfacecontact, -directionallights[ d ].direction );
			Intersect( shadowray, shadowtrace, primitive );
			if ( shadowtrace.closesthit ) {
				continue;
			}
			directional += rayshader( ray, trace, directionallights[ d ] );
		}
		for ( std::size_t p = 0; p < pointlights.size( ); ++p ) {
			Vec3 dir = surfacecontact.direction_to( pointlights[ p ].position );
			Ray shadowray( surfacecontact, dir );
			Intersect( shadowray, shadowtrace, primitive );
			if ( shadowtrace.closesthit ) {
				continue;
			}
			point += rayshader( ray, trace, pointlights[ p ] );
		}

		color = ambient + point + directional;
		
		return color.lerp( shadowcolor, shadowpower );
	}

};
