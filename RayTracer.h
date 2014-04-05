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

	RealRgba ReflectionBounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, const PrimitiveHit& primitivehit, const std::pair<real, real>& fresnel, const real& ior1, const real& ior2, std::size_t depth ) {
		using namespace Fur::Colors;
		const static RealRgba nontransparent = RealRgba( White );
		const static RealRgba transparent = RealRgba( Transparent );
		
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hasreflectivity = material.transparency > nontransparent && material.reflectivitypower > transparent;
		if ( !hasreflectivity )
			return RealRgba{ };
		
		Vec3 reflectionraydir = reflect( ray.direction, hit.normal );
		Ray reflectionray( hit.contact + reflectionraydir * bias,
			reflectionraydir );
		return Bounce( reflectionray, scene, shader, trace, shadowtrace, primitivehit, depth + 1, reflection_ray_arg );
	}

	RealRgba RefractionBounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, const PrimitiveHit& primitivehit, const std::pair<real, real>& fresnel, const real& ior1, const real& ior2, std::size_t depth ) {
		using namespace Fur::Colors;
		const static RealRgba nontransparent = RealRgba( White );

		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hastransparency = material.transparency < nontransparent;
		if ( !hastransparency )
			return RealRgba{ };

		Fur::optional<Vec3> oprefractionraydir = refract( ray.direction, hit.normal, ior1, ior2 );
		if ( !oprefractionraydir )
			return RealRgba{ };

		// Not total internal reflection
		const Vec3& refractionraydir = *oprefractionraydir;
		Ray refractionray( hit.contact + refractionraydir * bias, refractionraydir );
		return Bounce( refractionray, scene, shader, trace, shadowtrace, primitivehit, depth + 1, refraction_ray_arg );
	}

public:

	RayTracer( std::size_t max = 3, real raybias = static_cast<real>( 1e-4 ) )
	: bias( raybias ), maxdepth( max ) {

	}

	template <typename TRayId = primary_ray_arg_t>
	RealRgba Bounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0, TRayId rayid = TRayId{ } ) {
		using namespace Fur::Colors;
		const static RealRgba whitepoint = RealRgba( White );
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
		const Hit& hit = primitivehit.third;
		RealRgba opacity = whitepoint - material.transparency;
		bool nomoredepth = !( depth < maxdepth );
		bool hastransparency = material.transparency < whitepoint;
		bool hasreflectivity = material.transparency > whitepoint && material.reflectivitypower > transparent;

		RealRgba surface = scene.Lighting( ray, shader, primitivehit, shadowtrace );
		if ( ( !hasreflectivity && !hastransparency ) || nomoredepth ) {
			// Opaque object -- light it up!
			return surface;
		}

		const real& ior1 = previoushit ? previoushit->second.indexofrefraction : Ior::Vacuum;
		const real& ior2 = material.indexofrefraction;
		std::pair<real, real> fresnelfactors = fresnel( ray.direction, hit.normal, ior1, ior2 );
		
		RealRgba reflectioncolor = ReflectionBounce( ray, scene, shader, trace, shadowtrace, primitivehit, fresnelfactors, ior1, ior2, depth );
		RealRgba refractioncolor = RefractionBounce( ray, scene, shader, trace, shadowtrace, primitivehit, fresnelfactors, ior2, ior2, depth );

		sample = ( reflectioncolor * fresnelfactors.first * material.transparency )
			+ ( refractioncolor * fresnelfactors.second * opacity )
			;//+ ( surface * material.transparency );

		return sample;
	}

};
