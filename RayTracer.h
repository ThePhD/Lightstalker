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

	RayTracer( std::size_t max = 3, real raybias = static_cast<real>( 1e-4 ) )
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
		auto ophit = scene.Intersect( ray, trace );
		
		// The vacuum of the scene is the background of the scene: the diffuse component contains the background (all taken care of by Scene)
		if ( ophit->first.id == PrimitiveId::Vacuum ) {
			// If we're trying to compute a reflection,
			// then we shouldn't be using the vacuum's
			// background wall color -- just void
			// otherwise, if this is a primary ray that hits nothing
			// or if its a refraction ray (transparency ray),
			// then we want to have the vacuum act as a background color
			// and "shine through"
			return std::is_same<reflection_ray_arg_t, TRayId>::value 
				? Transparent 
				: ophit->second.diffuse;
		}

		PrimitiveHit& primitivehit = *ophit;
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		Hit& hit = primitivehit.third;
		bool hastransparency = material.transparency > transparent;
		bool hasreflectivity = material.reflectivity > transparent;

		RealRgba surface = scene.Lighting( ray, shader, primitivehit, shadowtrace );
		if ( ( !hasreflectivity && !hastransparency ) || maxdepth < depth ) {
			// Opaque object -- light it up!
			return surface;
		}

		real nreflectionratio = ray.direction.dot( hit.normal );
		real reflectionratio = -nreflectionratio;
		// Literally making shit up.
		real fresnelreflected = static_cast<real>( 0.5 );
		real fresnelrefracted = one - fresnelreflected;
		RealRgba opacity = static_cast<real>( 1 ) - material.transparency;

		RealRgba reflection{ };
		RealRgba refraction{ };

		if ( hasreflectivity ) {
			Vec3 reflectionraydir = Fur::normalize( ray.direction - hit.normal * two * nreflectionratio );
			Ray reflectionray( hit.contact + reflectionraydir * bias,
				reflectionraydir );
			reflection = Bounce( reflectionray, scene, shader, trace, shadowtrace, depth + 1, reflection_ray_arg );
		}
		if ( hastransparency ) {
			Vec3 refractionraydir = ray.direction;
			Ray refractionray( hit.contact + refractionraydir * bias, refractionraydir );
			refraction = Bounce( refractionray, scene, shader, trace, shadowtrace, depth + 1, refraction_ray_arg );
		}
		
		RealRgba refractioncomponent{ };
		refractioncomponent = refraction * fresnelrefracted * material.transparency;
		RealRgba reflectioncomponent{ };
		//reflectioncomponent = reflection * fresnelreflected;
		sample = reflectioncomponent 
			+ refractioncomponent
			/*+ surface * opacity*/;

		return sample;
	}

};
