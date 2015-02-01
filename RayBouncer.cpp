#include "RayBouncer.hpp"

RayBounce RayBouncer::Bounce( vec2 xy, size2 wh, const Camera& camera, const Scene& scene, const RayShader& shader ) const {
	Ray ray = camera.Compute( xy, wh );
	return Bounce( ray, scene, shader );
}

RayBounce RayBouncer::Bounce( const Ray& ray, const Scene& scene, const RayShader& shader ) const {
	RayBounce raybounce{ };
	Bounce( raybounce, ray, scene, shader, Fur::nullopt, 0 );
	++raybounce.samples;
	return raybounce;
}

void RayBouncer::Bounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, Fur::optional<const PrimitiveHit&> previoushit /*= Fur::nullopt*/, std::size_t depth /*= 0 */ ) const {
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
		rgba absorbance = material.color * material.absorption * -hit.distance0;
		std::transform( absorbance.begin( ), absorbance.end( ), 
			absorbance.begin( ), static_cast<real( * )( real )>( std::exp ) );
		rgba color = refractionbounce.color * absorbance;
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

RayBouncer::RayBouncer( std::size_t max /*= 3*/ ) : maxdepth( max ) {

}

bool RayBouncer::RefractionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const {
	const Primitive& primitive = primitivehit.first;
	const PrecalculatedMaterial& material = primitivehit.second;
	const Hit& hit = primitivehit.third;

	bool hastransparency = std::any_of( material.refractivity.begin( ), material.refractivity.end( ), real_compare<std::less<>, 1>( ) );
	if ( !hastransparency )
		return false;

	Fur::optional<vec3> oprefractionraydir = refract( ray.direction, hit.inside ? hit.normal * real_neg_one : hit.normal, ior1, ior2 );
	if ( !oprefractionraydir )
		return false;

	// Not total internal reflection
	const vec3& refractionraydir = *oprefractionraydir;
	Ray refractionray( hit.contact + refractionraydir * scene.Bias(), refractionraydir );
	raybounce.ray = refractionray;
	Bounce( raybounce, refractionray, scene, shader, primitivehit, depth + 1 );
	return true;
}

bool RayBouncer::ReflectionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const {
	raybounce.ray = ray;
	const Primitive& primitive = primitivehit.first;
	const PrecalculatedMaterial& material = primitivehit.second;
	const Hit& hit = primitivehit.third;

	bool hasreflectivity = std::any_of( material.reflectivity.begin( ), material.reflectivity.end( ), real_compare<std::greater<>, 0>( ) );
	if ( !hasreflectivity )
		return false;

	vec3 reflectionraydir = reflect( ray.direction, hit.normal );
	Ray reflectionray( hit.contact + reflectionraydir * scene.Bias(),
		reflectionraydir );
	Bounce( raybounce, reflectionray, scene, shader, primitivehit, depth + 1 );
	return true;
}
