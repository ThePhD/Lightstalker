#pragma once

#include "real.hpp"
#include "rgba.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "RayShader.hpp"
#include "RayBounce.hpp"
#include <functional>

class RayBouncer {
private:
	std::size_t maxdepth;
	
	bool ReflectionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const;

	bool RefractionBounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, const PrimitiveHit& primitivehit, const real& ior1, const real& ior2, std::size_t depth ) const;

	void Bounce( RayBounce& raybounce, const Ray& ray, const Scene& scene, const RayShader& shader, Fur::optional<const PrimitiveHit&> previoushit = Fur::nullopt, std::size_t depth = 0 ) const;
	
public:
	RayBouncer( std::size_t max = 3 );

	RayBounce Bounce( const Ray& ray, const Scene& scene, const RayShader& shader ) const;

	RayBounce Bounce( vec2 xy, size2 wh, const Camera& camera, const Scene& scene, const RayShader& shader ) const;

};
