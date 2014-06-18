#pragma once

#include "PrimitiveHit.h"
#include "Material.h"
#include "rgba.h"
#include "Light.h"
#include "RayShader.h"
#include <Furrovine++/optional.h>
#include <Furrovine++/reference_equals.h>
#include <Furrovine++/buffer_view.h>
#include <Furrovine++/BoundingBox.h>
#include <vector>

class Scene {
private:
	BoundingBox box;
	Primitive vacuumprimitive;
	Material vacuummaterial;
	Hit vacuumhit;
	std::vector<Primitive> primitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;

	void update_box( const Primitive& primitive ) {
		Triangle tri;
		switch ( primitive.id ) {
		case PrimitiveId::VertexTriangle:
			tri.a = primitive.meshtriangle.a.position;
			tri.b = primitive.meshtriangle.b.position;
			tri.c = primitive.meshtriangle.c.position;
			box.max.max( tri.maximum( ) );
			box.min.min( tri.minimum( ) );
			break;
		case PrimitiveId::Triangle:
			box.max.max( primitive.triangle.maximum( ) );
			box.min.min( primitive.triangle.minimum( ) );
			break;
		case PrimitiveId::Sphere:
			box.max.max( primitive.sphere.maximum( ) );
			box.min.min( primitive.sphere.minimum( ) );
			break;
		case PrimitiveId::Disk:
			
			break;
		case PrimitiveId::Plane:
			break;
		}
	}

public:

	Scene( const rgba& background = Fur::Colors::AmbientGrey ) 
	: vacuumprimitive( vacuum_arg ), vacuummaterial( BasicMaterial( background, RealWhite, RealTransparent, 0, RealWhite, RealTransparent, RealTransparent, Ior::Vacuum, Absorption::Vacuum, background ) ), vacuumhit( ) {
		vacuumhit.distance0 = vacuumhit.distance1 = std::numeric_limits<real>::max( );
		vacuumhit.normal = vec3::Zero;
		vacuumhit.stu = vec3( std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ) );
		vacuumhit.contact = vec3( std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ) );
		
		primitives.reserve( 9182 );
		materials.reserve( 9182 );
		ambientlights.reserve( 2 );
		pointlights.reserve( 8 );
		directionallights.reserve( 8 );
		AddMaterial( BasicMaterial( ) );
	}

	PrimitiveHit Vacuum( ) const {
		return PrimitiveHit{ vacuumprimitive, PrecalculatedMaterial( vacuummaterial, vacuumprimitive, vacuumhit ), vacuumhit };
	}

	void SetBackground( const rgba& background ) {
		vacuummaterial = std::move( Material( BasicMaterial( background, RealWhite, RealTransparent, 0, RealWhite, RealTransparent, RealTransparent, Ior::Vacuum, Absorption::Vacuum, background ) ) );
	}

	const rgba& Background( ) const {
		return vacuummaterial.color( vacuumprimitive, vacuumhit );
	}

	BoundingBox Bounds( ) const {
		return box;
	}

	template <typename Tm, typename... Tn>
	void Add( Tm&& material, Tn&&... argn ) {
		AddMaterial( std::forward<Tm>( material ) );
		AddPrimitive( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddMaterial( Tn&&... argn ) {
		materials.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddPrimitive( Tn&&... argn ) {
		primitives.emplace_back( std::forward<Tn>( argn )... );
		primitives.back( ).material = materials.size( ) - 1;
		update_box( primitives.back( ) );
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

	Fur::buffer_view<Material> Materials( ) {
		return materials;
	}

	Fur::buffer_view<Primitive> Primitives( ) {
		return primitives;
	}

	Fur::buffer_view<AmbientLight> AmbientLights( ) {
		return ambientlights;
	}

	Fur::buffer_view<DirectionalLight> DirectionalLights( ) {
		return directionallights;
	}

	Fur::buffer_view<PointLight> PointLights( ) {
		return pointlights;
	}

	const Primitive& PrimitiveAt( std::size_t idx ) const {
		return primitives[ idx ];
	}

	const Material& MaterialOf( Primitive& primitive ) const {
		return MaterialAt( primitive.material );
	}

	const Material& MaterialAt( std::size_t idx ) const {
		return materials[ idx ];
	}

	Fur::buffer_view<const Material> Materials( ) const {
		return materials;
	}

	Fur::buffer_view<const Primitive> Primitives( ) const {
		return primitives;
	}

	Fur::buffer_view<const AmbientLight> AmbientLights( ) const {
		return ambientlights;
	}

	Fur::buffer_view<const DirectionalLight> DirectionalLights( ) const {
		return directionallights;
	}

	Fur::buffer_view<const PointLight> PointLights( ) const {
		return pointlights;
	}

	/*Fur::buffer_view<const SpotLight> SpotLights( ) const {
	return spotlights;
	}

	Fur::buffer_view<SpotLight> SpotLights( ) {
		return spotlights;
	}*/


	Fur::optional<PrimitiveHit> Intersect( const Ray& ray, Fur::optional<const Primitive&> ignore = Fur::nullopt ) const {
		Fur::optional<PrimitiveHit> closesthit = Fur::nullopt;
		
		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			const Primitive& prim = primitives[ p ];
			if ( ignore && Fur::reference_equals( ignore.value( ), prim ) )
				continue;
			auto hit = intersect( ray, prim );
			if ( !hit )
				continue;
			if ( hit->distance0 < t0 ) {
				closesthit = PrimitiveHit{ prim, PrecalculatedMaterial( materials[ prim.material ], prim, hit.value() ), hit.value( ) };
				t0 = hit->distance0;
			}
		}

		if ( !closesthit )
			closesthit = Vacuum();

		return closesthit;
	}

	/*Fur::optional<PrimitiveHit> Intersect( const Ray& ray, RayTrace& trace, Fur::optional<const Primitive&> ignore = Fur::nullopt ) const {
		Fur::optional<PrimitiveHit> closesthit;
		closesthit = Fur::nullopt;
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
				closesthit = trace.hits.back();
				t0 = hit->distance0;
			}
		}

		if ( !closesthit )
			closesthit = Vacuum();

		trace.hits.push_back( Vacuum() );

		for ( std::size_t h = 0; h < trace.hits.size( ); ++h ) {
			trace.orderedhits.emplace_back( std::addressof( trace.hits[ h ] ) );
		}
		std::sort( trace.orderedhits.begin( ), trace.orderedhits.end( ), 
		[ ] ( PrimitiveHit* left, PrimitiveHit* right ) {
			return left->third.distance0 < right->third.distance0;
		} );

		return closesthit;
	}*/

};
