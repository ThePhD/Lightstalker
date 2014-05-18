#pragma once

#include "PrimitiveHit.h"
#include "rgba.h"
#include "Light.h"

struct RayShader {
public:
	Fur::triple<rgba, bool, bool> Shadow( const Ray& shadowray, const Scene& scene, const PrimitiveHit& primitivehit ) const;

	std::pair<rgba, bool> operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit ) const;

	std::pair<rgba, bool> operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const PointLight& pointlight ) const;

	std::pair<rgba, bool> operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const DirectionalLight& directionallight ) const;

	std::pair<rgba, bool> operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const AmbientLight& ambientlight ) const;

};