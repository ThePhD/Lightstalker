#include "Scene.h"

void Scene::Intersect( RayBounce& raybounce ) const {
	const Ray& ray = raybounce.ray;
#if NOKDTREE
	// Non-kd-tree intersection
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
		closesthit = Vacuum( );

	raybounce.hit = std::move( *closesthit );
#else
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
	if ( !raybounce.hit )
		raybounce.hit = Vacuum( );
#endif
}

void Scene::Build( ) {
#if NOKDTREE

#else
	kdtree.reset( new KdTree( box, primitives ) );
#endif
}

std::size_t Scene::PointLightCount( ) const {
	return pointlights.size( );
}

std::size_t Scene::DirectionalLightCount( ) const {
	return directionallights.size( );
}

std::size_t Scene::AmbientLightCount( ) const {
	return ambientlights.size( );
}

std::size_t Scene::PrimitiveCount( ) const {
	return primitives.size( );
}

std::size_t Scene::MaterialCount( ) const {
	return materials.size( );
}

Fur::buffer_view<const PointLight> Scene::PointLights( ) const {
	return pointlights;
}

Fur::buffer_view<PointLight> Scene::PointLights( ) {
	return pointlights;
}

Fur::buffer_view<const DirectionalLight> Scene::DirectionalLights( ) const {
	return directionallights;
}

Fur::buffer_view<DirectionalLight> Scene::DirectionalLights( ) {
	return directionallights;
}

Fur::buffer_view<const AmbientLight> Scene::AmbientLights( ) const {
	return ambientlights;
}

Fur::buffer_view<AmbientLight> Scene::AmbientLights( ) {
	return ambientlights;
}

Fur::buffer_view<const Primitive> Scene::Primitives( ) const {
	return primitives;
}

Fur::buffer_view<Primitive> Scene::Primitives( ) {
	return primitives;
}

Fur::buffer_view<const Material> Scene::Materials( ) const {
	return materials;
}

Fur::buffer_view<Material> Scene::Materials( ) {
	return materials;
}

const Material& Scene::MaterialAt( std::size_t idx ) const {
	return materials[ idx ];
}

Material& Scene::MaterialAt( std::size_t idx ) {
	return materials[ idx ];
}

const Material& Scene::MaterialOf( Primitive& primitive ) const {
	return MaterialAt( primitive.material );
}

Material& Scene::MaterialOf( Primitive& primitive ) {
	return MaterialAt( primitive.material );
}

const Primitive& Scene::PrimitiveAt( std::size_t idx ) const {
	return primitives[ idx ];
}

Primitive& Scene::PrimitiveAt( std::size_t idx ) {
	return primitives[ idx ];
}

Box Scene::Bounds( ) const {
	return box;
}

real Scene::Bias( ) const {
	return raybias;
}

const rgba& Scene::Background( ) const {
	return vacuummaterial.color( vacuumprimitive, vacuumhit );
}

void Scene::SetBackground( const rgba& background ) {
	vacuummaterial = std::move( Material( BasicMaterial( background, RealWhite, RealTransparent, 0, RealWhite, RealTransparent, RealTransparent, Ior::Vacuum, Absorption::Vacuum, background ) ) );
}

PrimitiveHit Scene::Vacuum( ) const {
	return PrimitiveHit{ vacuumprimitive, PrecalculatedMaterial( vacuummaterial, vacuumprimitive, vacuumhit ), vacuumhit };
}

Scene::Scene( const rgba& background /*= Fur::Colors::AmbientGrey */, real raybias ) : raybias( raybias ), vacuumprimitive( vacuum_arg ),
vacuummaterial( BasicMaterial( background, RealWhite, RealTransparent, 0, RealWhite, RealTransparent, RealTransparent, Ior::Vacuum, Absorption::Vacuum, background ) ),
vacuumhit( ),
box( ),
kdtree( nullptr ) {
	vacuumhit.distance0 = vacuumhit.distance1 = std::numeric_limits<real>::max( );
	vacuumhit.normal = vec3::Zero;
	vacuumhit.uvw = vec3::Zero;
	vacuumhit.contact = vec3( std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ), std::numeric_limits<real>::max( ) );

	primitives.reserve( 9182 );
	materials.reserve( 9182 );
	ambientlights.reserve( 2 );
	pointlights.reserve( 8 );
	directionallights.reserve( 8 );
	AddMaterial( BasicMaterial( ) );
}

void Scene::update_box( const Primitive& primitive ) {
	primitive.enclose_by( box );
}
