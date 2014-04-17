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

	void ReflectionBounce( std::vector<RealRgba>& bounces, const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) {
		using namespace Fur::Colors;
		const static RealRgba nontransparent = RealRgba( White );
		const static RealRgba transparent = RealRgba( Transparent );
		
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hasreflectivity = material.transparency > nontransparent || material.reflectivitypower > transparent;
		if ( !hasreflectivity )
			return;
		
		Vec3 reflectionraydir = reflect( ray.direction, hit.normal );
		Ray reflectionray( hit.contact + reflectionraydir * bias,
			reflectionraydir );
		RayBounce( bounces, reflectionray, scene, shader, trace, shadowtrace, primitivehit, depth + 1/*, reflection_ray_arg*/ );
	}

	RealRgba RefractionBounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) {
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
		return Bounce( refractionray, scene, shader, trace, shadowtrace, primitivehit, depth + 1/*, refraction_ray_arg*/ );
	}

	void RayBounce( std::vector<RealRgba>& bounces, const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) {
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
			bounces.push_back( ophit->second.diffuse );
			return;
		}

		PrimitiveHit& primitivehit = *ophit;
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;
		RealRgba opacity = whitepoint - material.transparency;
		bool nomoredepth = !( depth < maxdepth );
		bool hastransparency = material.transparency < whitepoint;
		bool hasreflectivity = material.transparency > whitepoint || material.reflectivitypower > transparent;

		RealRgba surface = scene.Lighting( ray, shader, primitivehit, shadowtrace );
		if ( ( !hasreflectivity && !hastransparency ) || nomoredepth ) {
			// Opaque object -- light it up!
			bounces.push_back( surface );
			return;
		}

		const real& ior1 = previoushit ? previoushit->second.indexofrefraction : Ior::Vacuum;
		const real& ior2 = material.indexofrefraction;
		std::pair<real, real> fresnelfactors = fresnel( ray.direction, hit.normal, ior1, ior2 );
		if ( fresnelfactors.first < 0 || fresnelfactors.first > 1
			|| fresnelfactors.second < 0 || fresnelfactors.second > 1 )
			bool fuck = true;

		std::size_t reflectionindex = bounces.size( );
		ReflectionBounce( bounces, ray, scene, shader, trace, shadowtrace, primitivehit, ior1, ior2, depth );
		//RealRgba refractioncolor = RefractionBounce( ray, scene, shader, trace, shadowtrace, primitivehit, ior2, ior2, depth );

		sample = ( bounces[ reflectionindex ] * fresnelfactors.first * material.reflectivitypower )
			;//+ ( refractioncolor * fresnelfactors.second * opacity )
			//+ ( surface * fresnelfactors.second );

		bounces.push_back( sample );
		return;
	}

public:

	RayTracer( std::size_t max = 3, real raybias = static_cast<real>( 1e-4 ) )
	: bias( raybias ), maxdepth( max ) {

	}

	RealRgba Bounce( const Ray& ray, const Scene& scene, RayShader& shader, RayTrace& trace, RayTrace& shadowtrace, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) {
		std::vector<RealRgba> bounces;
		RayBounce( bounces, ray, scene, shader, trace, shadowtrace, previoushit, depth );
		return bounces.back();
	}

};
