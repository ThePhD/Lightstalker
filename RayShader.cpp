#include "RayShader.h"
#include "Scene.h"

std::pair<rgba, bool> RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const AmbientLight& ambientlight ) const {
	return{ rgba( ambientlight.intensity * primitivehit.second.ambient ), false };
}

std::pair<rgba, bool> RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const DirectionalLight& directionallight ) const {
	const Primitive& primitive = primitivehit.first;
	const PrecalculatedMaterial& material = primitivehit.second;
	const Hit& hit = primitivehit.third;

	vec3 directiontolight = -directionallight.direction;
	Ray shadowray( hit.contact, directiontolight );
	auto shadowshaded = Shadow( shadowray, scene, primitivehit );
	rgba& shadow = shadowshaded.first;
	if ( shadowshaded.third ) {
		return{ shadow, true };
	}

	rgba color( 0, 0, 0, 0 );

	// Diffuse Term
	real brightness = dot( hit.normal, directiontolight );
	if ( brightness > static_cast<real>( 0 ) 
		&& material.diffuse > RealTransparent ) {
		color += material.diffuse * brightness * material.color * directionallight.intensity;
	}

	// Specular Term
	if ( material.specular > RealTransparent ) {
		vec3 halfway = ( 2 * brightness * hit.normal ) - directiontolight;
		real normaldothalfway = dot( -ray.direction, halfway );
		if ( normaldothalfway >= static_cast<real>( 0 ) ) {
			rgba specularbrightness = std::pow( normaldothalfway, material.specularpower ) * material.specular;
			color += specularbrightness * directionallight.intensity;
		}
	}

	return{ color * shadow, shadowshaded.second };
}

std::pair<rgba, bool> RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const PointLight& pointlight ) const {
	const Hit& hit = primitivehit.third;
	DirectionalLight directionallight( hit.contact.direction_to( pointlight.position ), pointlight.intensity );
	auto& me = *this;
	return me( ray, scene, primitivehit, directionallight );
}

Fur::triple<rgba, bool, bool> RayShader::Shadow( const Ray& shadowray, const Scene& scene, const PrimitiveHit& primitivehit ) const {
	if ( !Shadows ) {
		return{ RealWhite, false, false };
	}
	auto shadowophit = scene.Intersect( shadowray, primitivehit.first );
	if ( !shadowophit || shadowophit->first.id == PrimitiveId::Vacuum )
		return { RealWhite, false, false };
	auto shadow = shadowophit->second.refractivity;
	bool returnimmediately = !( shadow > RealTransparent );
	return { shadow, true, returnimmediately };
}

std::pair<rgba, bool> RayShader::operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit ) const {
	auto ambientlights = scene.AmbientLights( );
	auto directionallights = scene.DirectionalLights( );
	auto pointlights = scene.PointLights( );
	real ambientsize = static_cast<real>( ambientlights.size( ) );
	real lightssize = static_cast<real>( directionallights.size( ) ) + static_cast<real>( pointlights.size( ) );
	bool shaded = false;
	rgba color{ };
	rgba ambientcolor{ };
	const Primitive& primitive = primitivehit.first;
	
	for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
		auto colorshadow = ( *this )( ray, scene, primitivehit, ambientlights[ a ] );
		ambientcolor += colorshadow.first / ambientsize;
		shaded |= colorshadow.second;
	}

	for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
		auto colorshadow = ( *this )( ray, scene, primitivehit, directionallights[ d ] );
		color += colorshadow.first / lightssize;
		shaded |= colorshadow.second;
	}

	for ( std::size_t p = 0; p < pointlights.size( ); ++p ) {
		auto colorshadow = ( *this )( ray, scene, primitivehit, pointlights[ p ] );
		color += colorshadow.first / lightssize;
		shaded |= colorshadow.second;
	}
	color += ambientcolor;

	return { color, shaded };
}

RayShader::RayShader( ) : Shadows( true ) {

}
