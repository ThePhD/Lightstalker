#pragma once

#include "real.h"
#include "RealRgba.h"
#include "Primitive.h"
#include "Scene.h"
#include "RayTrace.h"
#include "RayShader.h"

struct primary_ray_arg_t { };
const auto primary_ray_arg = primary_ray_arg_t{ };
struct refraction_ray_arg_t { };
const auto refraction_ray_arg = refraction_ray_arg_t{ };
struct reflection_ray_arg_t { };
const auto reflection_ray_arg = reflection_ray_arg_t{ };

class RayTracer {
private:
	real bias;
	std::size_t maxdepth;

	template<typename T>
	T transmission_coefficients( const T &a, const T &b, const T &mix ) {
		return b * mix + a * ( T( 1 ) - mix );
	}

public:

	RayTracer( std::size_t max = 2, real raybias = static_cast<real>( 1e-5 ) )
	: bias( raybias ), maxdepth( max ) {

	}

	template <typename TRayId = primary_ray_arg_t>
	RealRgba Bounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, std::size_t depth = 0, TRayId rayid = TRayId{ } ) {
		using namespace Fur::Colors;
		const static RealRgba transparent = RealRgba( Transparent );
		const static real two = static_cast<real>( 2 );
		const static real one = static_cast<real>( 1 );
		const static real zero = static_cast<real>( 0 );
		RealRgba sample{ };
		scene.Intersect( ray, trace );
		
		// The vacuum of the scene is the background of the scene: the diffuse component contains the background (all taken care of by Scene)
		if ( trace.closesthit->first.id == PrimitiveId::Vacuum ) {
			// If we're trying to compute a reflection,
			// then we shouldn't be using the vacuum's
			// background wall color -- just void
			// otherwise, if this is a primary ray that hits nothing
			// or if its a refraction ray (transparency ray),
			// then we want to have the vacuum act as a background color
			// and "shine through"
			return std::is_same<reflection_ray_arg_t, TRayId>::value 
				? Transparent 
				: trace.closesthit->second.diffuse;
		}

		PrimitiveHit& primitivehit = *trace.closesthit;
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		Hit& hit = primitivehit.third;
		bool hastransparency = material.transparency > transparent;
		bool hasreflectivity = material.reflectivity > transparent;

		RealRgba surface = scene.Lighting( ray, shader, primitivehit, shadowtrace );
		if ( maxdepth < depth || !hasreflectivity && !hastransparency ) {
			// Opaque object -- light it up!
			return surface;
		}

		real nreflectionratio = ray.direction.dot( hit.normal );
		real reflectionratio = -nreflectionratio;
		// Literally making shit up.
		real fresnelreflected = transmission_coefficients<real>( static_cast<real>( std::pow( 1 - reflectionratio, 3 ) ), one, static_cast<real>( 0.1 ) );
		real fresnelrefracted = one - fresnelreflected;

		Ray reflectionray( hit.contact + hit.normal * bias,
			Fur::normalize( ray.direction - hit.normal * two * nreflectionratio ) );
		RealRgba reflection = Bounce( reflectionray, scene, shader, trace, shadowtrace, depth + 1, reflection_ray_arg );
		RealRgba refraction{ };
		
		RealRgba refractioncomponent = material.transparency * fresnelrefracted * refraction;
		RealRgba reflectioncomponent = material.reflectivity * fresnelreflected * reflection;
		sample = reflectioncomponent + refractioncomponent
			// Surface compnent is all lights * ( diffuse + specularity + ambient )
			+ surface;

		return sample;
	}

};
