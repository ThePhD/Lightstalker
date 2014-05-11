#include "RayShader.h"
#include "Scene.h"

rgba RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const AmbientLight& ambientlight ) const {
	return rgba( ambientlight.intensity * primitivehit.second.ambient( primitivehit.first, primitivehit.third ) );
}

rgba RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const DirectionalLight& directionallight ) const {
	const Primitive& primitive = primitivehit.first;
	const Material& material = primitivehit.second;
	const Hit& hit = primitivehit.third;

	vec3 directiontolight = -directionallight.direction;
	Ray shadowray( hit.contact, directiontolight );
	rgba shadow = RealWhite;
	if ( Shadow( shadow, shadowray, scene, primitivehit ) ) {
		return shadow;
	}

	rgba color( 0, 0, 0, 0 );

	// Diffuse Term
	real brightness = dot( hit.normal, directiontolight );
	if ( brightness > static_cast<real>( 0 ) 
		&& material.diffuse( primitive, hit ) > RealTransparent ) {
		color += material.diffuse( primitive, hit ) * brightness * material.color( primitive, hit ) * directionallight.intensity * shadow;
	}

	// Specular Term
	if ( material.specular( primitive, hit ) > RealTransparent ) {
		vec3 halfway = ( 2 * brightness * hit.normal ) - directiontolight;
		real normaldothalfway = dot( -ray.direction, halfway );
		if ( normaldothalfway >= static_cast<real>( 0 ) ) {
			rgba specularbrightness = std::pow( normaldothalfway, material.specularpower( primitive, hit ) ) * material.specular( primitive, hit ) * shadow;
			color += specularbrightness * directionallight.intensity;
		}
	}

	return color;
}

rgba RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const PointLight& pointlight ) const {
	const Hit& hit = primitivehit.third;
	DirectionalLight directionallight( hit.contact.direction_to( pointlight.position ), pointlight.intensity );
	auto& me = *this;
	return me( ray, scene, primitivehit, directionallight );
}

bool RayShader::Shadow( rgba& shadow, const Ray& shadowray, const Scene& scene, const PrimitiveHit& primitivehit ) const {
	auto shadowophit = scene.Intersect( shadowray, primitivehit.first );
	if ( !shadowophit || shadowophit->first.id == PrimitiveId::Vacuum )
		return false;
	shadow = RealTransparent;
	return true;
}

rgba RayShader::Lighting( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit ) const {
	auto ambientlights = scene.AmbientLights( );
	auto directionallights = scene.DirectionalLights( );
	auto pointlights = scene.PointLights( );

	real perambient = static_cast<real>( 1 ) / ambientlights.size( );
	real perlight = static_cast<real>( 1 ) / ( pointlights.size( ) + directionallights.size( ) );
	rgba color{ };

	rgba ambient{ };
	rgba directional{ };
	rgba point{ };

	const Primitive& primitive = primitivehit.first;
	const Material& material = primitivehit.second;
	const Hit& hit = primitivehit.third;
	const vec3& surfacecontact = hit.contact;

	for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
		ambient += ( *this )( ray, scene, primitivehit, ambientlights[ a ] );// *perambient;
	}

	for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
		directional += ( *this )( ray, scene, primitivehit, directionallights[ d ] );// *perlight;
	}

	for ( std::size_t p = 0; p < pointlights.size( ); ++p ) {
		point += ( *this )( ray, scene, primitivehit, pointlights[ p ] );// *perlight;
	}

	color += ambient + point + directional;
	return color;
}
