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
	real raybias;
	std::vector<Primitive> primitives;
	std::vector<Primitive> unboundedprimitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;
	std::unique_ptr<KdTree> kdtree;

	void update_box( const Primitive& primitive );

public:

	Scene( const rgba& background = Fur::Colors::AmbientGrey, real raybias = static_cast<real>( 1e-2 ) );

	PrimitiveHit Vacuum( ) const;

	void SetBackground( const rgba& background );

	const rgba& Background( ) const;

	Box Bounds( ) const;

	real Bias( ) const;

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
			update_box( unboundedprimitives.back( ) );
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

	Primitive& PrimitiveAt( std::size_t idx );

	Material& MaterialOf( Primitive& primitive );

	Material& MaterialAt( std::size_t idx );

	Fur::buffer_view<Material> Materials( );

	Fur::buffer_view<Primitive> Primitives( );

	Fur::buffer_view<AmbientLight> AmbientLights( );

	Fur::buffer_view<DirectionalLight> DirectionalLights( );

	Fur::buffer_view<PointLight> PointLights( );

	const Primitive& PrimitiveAt( std::size_t idx ) const;

	const Material& MaterialOf( Primitive& primitive ) const;

	const Material& MaterialAt( std::size_t idx ) const;

	Fur::buffer_view<const Material> Materials( ) const;

	Fur::buffer_view<const Primitive> Primitives( ) const;

	Fur::buffer_view<const AmbientLight> AmbientLights( ) const;

	Fur::buffer_view<const DirectionalLight> DirectionalLights( ) const;

	Fur::buffer_view<const PointLight> PointLights( ) const;

	std::size_t MaterialCount( ) const;

	std::size_t PrimitiveCount( ) const;

	std::size_t AmbientLightCount( ) const;

	std::size_t DirectionalLightCount( ) const;

	std::size_t PointLightCount( ) const;

	/*Fur::buffer_view<const SpotLight> SpotLights( ) const {
		return spotlights;
	}

	Fur::buffer_view<SpotLight> SpotLights( ) {
		return spotlights;
	}*/

	void Build( );

	void Intersect( RayBounce& raybounce ) const;
};
