#pragma once

#include "Primitive.h"
#include "RealRgba.h"
#include "RayTrace.h"
#include <vector>

struct RayShader {
public:

	RealRgba operator()( const Ray& ray, PrimitiveHit& primitivehit, const PointLight& pointlight ) {
		Hit& hit = primitivehit.third;
		DirectionalLight directionallight( hit.contact.direction_to( pointlight.position ), pointlight.intensity );
		auto& me = *this;
		return me( ray, primitivehit, directionallight );
	}

	RealRgba operator()( const Ray& ray, PrimitiveHit& primitivehit, const DirectionalLight& directionallight ) {
		RealRgba color{ };

		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		Hit& hit = primitivehit.third;
		Vec3 viewerray = -ray.direction;
		Vec3 directiontolight = -directionallight.direction;
		
		// Diffuse Term
		real brightness = dot( hit.normal, directiontolight );
		real clampedbrightness = Fur::clamp( brightness, static_cast<real>( 0 ), static_cast<real>( 1 ) );
		color += material.diffuse * clampedbrightness;
		
		// Specular Term
		Vec3 halfway = ( 2 * brightness * hit.normal ) - directiontolight;
		real normaldothalfway = dot( viewerray, halfway );
		if ( normaldothalfway >= static_cast<real>( 0 ) ) {
			real specularbrightness = std::pow( normaldothalfway, material.specularpower );
			real specularclampedbrightness = Fur::clamp( specularbrightness, static_cast<real>( 0 ), static_cast<real>( 1 ) );
			color += material.specularity * specularclampedbrightness;
		}

		return color;
	}

	RealRgba operator()( const Ray& ray, PrimitiveHit& primitivehit, const AmbientLight& ambientlight ) {
		return RealRgba( ambientlight * primitivehit.second.ambient );
	}

};