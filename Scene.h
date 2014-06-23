#pragma once

#include "PrimitiveHit.h"
#include "Material.h"
#include "rgba.h"
#include "Light.h"
#include "RayShader.h"
#include "RayBounce.h"
#include "KdTree.h"
#include <Furrovine++/optional.h>
#include <Furrovine++/reference_equals.h>
#include <Furrovine++/buffer_view.h>
#include <Furrovine++/enclose3.h>
#include <vector>

class Scene {
private:
	Box box;
	Primitive vacuumprimitive;
	Material vacuummaterial;
	Hit vacuumhit;
	std::vector<Primitive> primitives;
	std::vector<Primitive> unboundedprimitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;
	std::unique_ptr<KdTree> kdtree;

	void update_box( const Primitive& primitive ) {
		primitive.enclose_by( box );
	}

public:

	Scene( const rgba& background = Fur::Colors::AmbientGrey ) 
	: vacuumprimitive( vacuum_arg ), 
	vacuummaterial( BasicMaterial( background, RealWhite, RealTransparent, 0, RealWhite, RealTransparent, RealTransparent, Ior::Vacuum, Absorption::Vacuum, background ) ), 
	vacuumhit( ), 
	box(),
	kdtree( nullptr ) {
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

	Box Bounds( ) const {
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
		Primitive primitive( std::forward<Tn>( argn )... );
		if ( primitive.id == PrimitiveId::Plane ) {
			unboundedprimitives.push_back( primitive );
			Primitive& primitive = unboundedprimitives.back( );
			primitive.material = materials.size( ) - 1;
			update_box( primitives.back( ) );
		}
		else {
			primitives.push_back( primitive );
			Primitive& primitive = primitives.back( );
			primitive.material = materials.size( ) - 1;
			update_box( primitives.back( ) );
		}
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

	std::size_t MaterialCount( ) const {
		return materials.size();
	}

	std::size_t PrimitiveCount( ) const {
		return primitives.size();
	}

	std::size_t AmbientLightCount( ) const {
		return ambientlights.size();
	}

	std::size_t DirectionalLightCount( ) const {
		return directionallights.size();
	}

	std::size_t PointLightCount( ) const {
		return pointlights.size();
	}

	/*Fur::buffer_view<const SpotLight> SpotLights( ) const {
		return spotlights;
	}

	Fur::buffer_view<SpotLight> SpotLights( ) {
		return spotlights;
	}*/

	void Build( ) {
		kdtree.reset( new KdTree( box, primitives ) );
	}

	void Intersect( RayBounce& raybounce ) const {
		const Ray& ray = raybounce.ray;
		kdtree->Intersect( raybounce, materials );
		for ( std::size_t p = 0; p < unboundedprimitives.size( ); ++p ) {
			const Primitive& prim = unboundedprimitives[ p ];
			auto hit = intersect( ray, prim );
			++raybounce.primitivetests;
			if ( !hit )
				continue;
			++raybounce.primitivehits;
			if ( !raybounce.hit || hit->distance0 < raybounce.hit->third.distance0 ) {
				raybounce.hit = PrimitiveHit{ prim, PrecalculatedMaterial( materials[ prim.material ], prim, hit.value( ) ), hit.value( ) };
				++raybounce.overlappingprimitivehits;
			}
		}
	}

#if 0
	// Non-kd-tree intersection
	void Intersect( RayBounce& raybounce ) const {
		const Ray& ray = raybounce.ray;
		Fur::optional<PrimitiveHit> closesthit = Fur::nullopt;
		
		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			const Primitive& prim = primitives[ p ];
			auto hit = intersect( ray, prim );
			++raybounce.primitivetests;
			if ( !hit )
				continue;
			++raybounce.primitivehits;
			if ( hit->distance0 < t0 ) {
				closesthit = PrimitiveHit{ prim, PrecalculatedMaterial( materials[ prim.material ], prim, hit.value( ) ), hit.value( ) };
				t0 = hit->distance0;
				++raybounce.overlappingprimitivehits;
			}
		}
		for ( std::size_t p = 0; p < unboundedprimitives.size( ); ++p ) {
			const Primitive& prim = unboundedprimitives[ p ];
			if ( ignore && Fur::reference_equals( ignore.value( ), prim ) )
				continue;
			auto hit = intersect( ray, prim );
			++raybounce.primitivetests;
			if ( !hit )
				continue;
			++raybounce.primitivehits;
			if ( hit->distance0 < t0 ) {
				closesthit = PrimitiveHit{ prim, PrecalculatedMaterial( materials[ prim.material ], prim, hit.value( ) ), hit.value( ) };
				t0 = hit->distance0;
				++raybounce.overlappingprimitivehits;
			}
		}

		if ( !closesthit )
			closesthit = Vacuum();

		raybounce.hit = std::move( *closesthit );
	}
#endif // Non-kd-tree

};
