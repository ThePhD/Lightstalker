#include "RayShader.hpp"
#include "Scene.hpp"

bool RayShader::Shadow( RayBounce& raybounce, const Scene& scene ) const {
	if ( !Shadows ) {
		return false;
	}
	scene.Intersect( raybounce );
	const Fur::optional<PrimitiveHit>& ophit = raybounce.hit;
	if ( !ophit )
		return false;
	const PrimitiveHit& primitivehit = *raybounce.hit;
	const Primitive& primitive = primitivehit.first;
	if ( primitive.id == PrimitiveId::Vacuum )
		return false;
	const PrecalculatedMaterial& material = primitivehit.second;
	return std::all_of( material.opacity.begin( ), material.opacity.end( ),
		real_compare<std::equal_to<>, 0>( ) );
}

void RayShader::operator()( RayBounce& raybounce, const Scene& scene, const AmbientLight& ambientlight ) const {
	const real ambientcontribution = 1 / static_cast<real>( scene.AmbientLightCount() );
	const PrimitiveHit& primitivehit = *raybounce.hit;
	const PrecalculatedMaterial& material = primitivehit.second;
	raybounce.color += ( ambientlight.intensity * material.ambient ) * ambientcontribution;
}

void RayShader::operator()( RayBounce& raybounce, const Scene& scene, const DirectionalLight& directionallight ) const {
	const real lightcontribution = 1 / ( static_cast<real>( scene.DirectionalLightCount( ) ) + static_cast<real>( scene.PointLightCount( ) ) );
	const Ray& ray = raybounce.ray;
	const PrimitiveHit& primitivehit = *raybounce.hit;
	const Primitive& primitive = primitivehit.first;
	const PrecalculatedMaterial& material = primitivehit.second;
	const Hit& hit = primitivehit.third;
	rgba color( 0, 0, 0, 0 );

	vec3 directiontolight = -directionallight.direction;
	Ray shadowray( hit.contact + ( directiontolight * scene.Bias() ),
		directiontolight );
	RayBounce shadowbounce{ };
	shadowbounce.ray = shadowray;
	if ( Shadow( shadowbounce, scene ) ) {
		raybounce.shadow = true;
		raybounce.hitid += static_cast<std::uintptr_t>( 1 );
		return;
	}

	// Diffuse Term
	real brightness = dot( hit.normal, directiontolight );
	if ( brightness > static_cast<real>( 0 ) ) {
		color += material.diffuse * brightness * material.color * directionallight.intensity;
	}

	// Specular Term
	vec3 halfway = ( 2 * brightness * hit.normal ) - directiontolight;
	real normaldothalfway = dot( -ray.direction, halfway );
	if ( normaldothalfway >= static_cast<real>( 0 ) ) {
		rgba specularbrightness = std::pow( normaldothalfway, material.specularpower ) * material.specular;
		color += specularbrightness * directionallight.intensity;
	}

	color *= lightcontribution;
	if ( shadowbounce.hit && shadowbounce.hit->first.id != PrimitiveId::Vacuum ) {
		const PrecalculatedMaterial& shadowmaterial = shadowbounce.hit->second;
		color.lerp( rgba::Transparent, shadowmaterial.opacity );
		raybounce.hitid += static_cast<std::uintptr_t>( 1 );
	}
	raybounce.color += color;
}

void RayShader::operator()( RayBounce& raybounce, const Scene& scene, const PointLight& pointlight ) const {
	const PrimitiveHit& primitivehit = *raybounce.hit;
	const Primitive& primitive = primitivehit.first;
	const Hit& hit = primitivehit.third;
	DirectionalLight directionallight( pointlight.position.direction_to( hit.contact ), pointlight.intensity );
	auto& me = *this;
	me( raybounce, scene, directionallight );
}

void RayShader::operator()( RayBounce& raybounce, const Scene& scene ) const {
	const PrimitiveHit& primitivehit = *raybounce.hit;
	const Primitive& primitive = primitivehit.first;
	const Hit& hit = primitivehit.third;
	auto ambientlights = scene.AmbientLights( );
	auto directionallights = scene.DirectionalLights( );
	auto pointlights = scene.PointLights( );
	
	for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
		( *this )( raybounce, scene, ambientlights[ a ] );
	}
	
	for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
		( *this )( raybounce, scene, directionallights[ d ] );
	}

	for ( std::size_t p = 0; p < pointlights.size( ); ++p ) {
		( *this )( raybounce, scene, pointlights[ p ] );
	}
}

RayShader::RayShader( ) : Shadows( true ) {

}
