#pragma once

#include "real.h"
#include "rgba.h"
#include "Primitive.h"
#include "Scene.h"
#include "RayShader.h"

class RayTracer {
private:
	real bias;
	std::size_t maxdepth;

	bool ReflectionBounce( rgba& bounces, const Ray& ray, const Scene& scene, RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) {
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hasreflectivity = material.reflectivity > RealTransparent;
		if ( !hasreflectivity )
			return false;
		
		Vec3 reflectionraydir = reflect( ray.direction, hit.normal );
		Ray reflectionray( hit.contact + reflectionraydir * bias,
			reflectionraydir );
		Bounce( bounces, reflectionray, scene, shader, primitivehit, depth + 1 );
		return true;
	}

	bool RefractionBounce( rgba& bounces, const Ray& ray, const Scene& scene, RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) {
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;

		bool hastransparency = material.refractivity > RealTransparent;
		if ( !hastransparency )
			return false;

		Fur::optional<Vec3> oprefractionraydir = refract( ray.direction, hit.inside ? hit.normal * real_neg_one : hit.normal, ior1, ior2 );
		if ( !oprefractionraydir )
			return false;

		// Not total internal reflection
		const Vec3& refractionraydir = *oprefractionraydir;
		Ray refractionray( hit.contact + refractionraydir * bias, refractionraydir );
		Bounce( bounces, refractionray, scene, shader, primitivehit, depth + 1 );
		return true;
	}

	void Bounce( rgba& bounces, const Ray& ray, const Scene& scene, RayShader& shader, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) {
		auto ophit = scene.Intersect( ray );
		if ( !ophit )
			return;

		const PrimitiveHit& primitivehit = *ophit;
		const Primitive& primitive = primitivehit.first;
		const Material& material = primitivehit.second;
		const Hit& hit = primitivehit.third;
		
		// The vacuum of the scene is the background of the scene: the diffuse component contains the background (all taken care of by Scene)
		if ( primitive.id == PrimitiveId::Vacuum ) {
			bounces = ophit->second.color;
			return;
		}

		rgba surface = shader.Lighting( ray, scene, primitivehit );
		if ( material.diffuse > RealTransparent ) {
			bounces += surface;
		}

		if ( !( depth < maxdepth ) )
			return;

		const real& ior1 = previoushit ? previoushit->second.indexofrefraction : Ior::Vacuum;
		const real& ior2 = material.indexofrefraction;
		
		// Refraction Part
		rgba refractioncolor( 0, 0, 0, 0 );
		if ( RefractionBounce( refractioncolor, ray, scene, shader, primitivehit, ior2, ior2, depth ) ) {
			// Things have a slight coloration to depth if they are shallow refractions.
			// Intensity of the light drops off related to density and concentration
			// of transparent material
			rgba absorbance = material.color * material.absorption * -hit.distance0;
			rgba opacity( std::exp( absorbance.r ),
				std::exp( absorbance.g ),
				std::exp( absorbance.b ),
				std::exp( absorbance.a ) );
			rgba color = refractioncolor * opacity * material.refractivity;
			bounces += color;
		}

		// Reflection Component
		rgba reflectioncolor( 0, 0, 0, 0 );
		if ( ReflectionBounce( reflectioncolor, ray, scene, shader, primitivehit, ior1, ior2, depth ) ) {
			rgba color = material.reflectivity * reflectioncolor * surface;
			bounces += color;
		}
	}
	
public:

	RayTracer( std::size_t max = 3, real raybias = static_cast<real>( 1e-4 ) )
	: bias( raybias ), maxdepth( max ) {

	}

	rgba Bounce( const Ray& ray, const Scene& scene, RayShader& shader, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) {
		rgba bounces( 0, 0, 0, 0 );
		Bounce( bounces, ray, scene, shader, previoushit, depth );
		return bounces;
	}

};
