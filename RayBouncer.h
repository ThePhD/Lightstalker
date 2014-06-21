#pragma once

#include "real.h"
#include "rgba.h"
#include "Camera.h"
#include "Scene.h"
#include "RayShader.h"
#include "RayBounce.h"
#include <functional>

class RayBouncer {
private:
	real bias;
	std::size_t maxdepth;
	
	bool ReflectionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const {
		raybounce.ray = ray;
		const Primitive& primitive = primitivehit.first;
		const PrecalculatedMaterial& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hasreflectivity = std::any_of( material.reflectivity.begin( ), material.reflectivity.end( ), real_compare<std::greater<>, 0>( ) );
		if ( !hasreflectivity )
			return false;
		
		vec3 reflectionraydir = reflect( ray.direction, hit.normal );
		Ray reflectionray( hit.contact + reflectionraydir * bias,
			reflectionraydir );
		Bounce( raybounce, reflectionray, scene, shader, primitivehit, depth + 1 );
		return true;
	}

	bool RefractionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const {
		const Primitive& primitive = primitivehit.first;
		const PrecalculatedMaterial& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hastransparency = std::any_of( material.refractivity.begin( ), material.refractivity.end( ), real_compare<std::less<>, 1>() );
		if ( !hastransparency )
			return false;

		Fur::optional<vec3> oprefractionraydir = refract( ray.direction, hit.inside ? hit.normal * real_neg_one : hit.normal, ior1, ior2 );
		if ( !oprefractionraydir )
			return false;

		// Not total internal reflection
		const vec3& refractionraydir = *oprefractionraydir;
		Ray refractionray( hit.contact + refractionraydir * bias, refractionraydir );
		raybounce.ray = refractionray;
		Bounce( raybounce, refractionray, scene, shader, primitivehit, depth + 1 );
		return true;
	}

	void Bounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) const {
		raybounce.ray = ray;
		raybounce.hitid = 0;
		scene.Intersect( raybounce );
		if ( !raybounce.hit )
			return;
		raybounce.hitid = reinterpret_cast<std::uintptr_t>( std::addressof( raybounce.hit->first ) );
		const PrimitiveHit& primitivehit = *raybounce.hit;
		const Primitive& primitive = primitivehit.first;
		const PrecalculatedMaterial& material = primitivehit.second;
		const Hit& hit = primitivehit.third;
		bool shaded = false;

		// The vacuum of the scene is the background of the scene: the diffuse component contains the background (all taken care of by Scene)
		if ( primitive.id == PrimitiveId::Vacuum ) {
			raybounce.color += raybounce.hit->second.color;
			return;
		}

		shader( raybounce, scene );
		raybounce.hitid += static_cast<std::uintptr_t>( raybounce.shadow );

		if ( depth >= maxdepth )
			return;

		const real& ior1 = previoushit ? previoushit->second.indexofrefraction : Ior::Vacuum;
		const real& ior2 = material.indexofrefraction;
		
		// Refraction Part
		RayBounce refractionbounce{ };
		if ( RefractionBounce( refractionbounce, ray, scene, shader, primitivehit, ior2, ior2, depth ) ) {
			// Things have a slight coloration to depth if they are shallow refractions.
			// Intensity of the light drops off related to density and concentration
			// of transparent material
			/*rgba absorbance = material.color( primitive, hit ) * material.absorption( primitive, hit ) * -hit.distance0;
			rgba opacity( std::exp( absorbance.r ),
				std::exp( absorbance.g ),
				std::exp( absorbance.b ),
				std::exp( absorbance.a ) );*/
			rgba color = refractionbounce.color;// *opacity;
			raybounce.color += color;
			++raybounce.refractions;
		}

		// Reflection Component
		RayBounce reflectionbounce{ };
		if ( ReflectionBounce( reflectionbounce, ray, scene, shader, primitivehit, ior1, ior2, depth ) ) {
			rgba color = material.reflectivity * reflectionbounce.color;
			raybounce.color += color;
			++raybounce.reflections;
		}
		raybounce.hitid += static_cast<std::uintptr_t>( raybounce.refractions > 0 );
		raybounce.hitid += static_cast<std::uintptr_t>( raybounce.reflections > 0 );
	}
	
public:
	RayBouncer( std::size_t max = 3, real raybias = static_cast<real>( 1e-2 ) )
	: bias( raybias ), maxdepth( max ) {

	}

	RayBounce Bounce( const Ray& ray, const Scene& scene, const RayShader& shader ) const {
		RayBounce raybounce{ };
		Bounce( raybounce, ray, scene, shader, Fur::nullopt, 0 );
		++raybounce.samples;
		return raybounce;
	}

	RayBounce Bounce( vec2 xy, size2 wh, const Camera& camera, const Scene& scene, const RayShader& shader ) const {
		Ray ray = camera.Compute( xy, wh );
		return Bounce( ray, scene, shader );
	}

};
