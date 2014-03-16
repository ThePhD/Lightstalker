#pragma once

#include "Primitive.h"
#include "RealRgba.h"
#include "Trace.h"
#include <vector>

struct RayShader {
public:

	RealRgba operator()( const Ray& ray, Trace& trace, const PointLight& pointlight ) {
		Hit& hit = trace.closesthit->third;
		DirectionalLight directionallight( hit.contact.direction_to( pointlight.position ) );
		auto& me = *this;
		return me( ray, trace, directionallight );
	}

	RealRgba operator()( const Ray& ray, Trace& trace, const DirectionalLight& directionallight ) {
		RealRgba color{ };
		Primitive& primitive = trace.closesthit->first;
		Material& material = trace.closesthit->second;
		Hit& hit = trace.closesthit->third;
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
			color += material.specular * specularclampedbrightness;
		}

		return color;
	}

	RealRgba operator()( const Ray& ray, Trace& trace, const AmbientLight& ambientlight ) {
		return RealRgba( ambientlight );
	}

};