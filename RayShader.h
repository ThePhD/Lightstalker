#pragma once

#include "PrimitiveHit.h"
#include "rgba.h"
#include "Light.h"

struct RayShader {
public:
	bool Shadow( rgba& shadow, const Ray& shadowray, const Scene& scene, const PrimitiveHit& primitivehit ) const;

	rgba Lighting( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit ) const;

	rgba operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const PointLight& pointlight ) const;

	rgba operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const DirectionalLight& directionallight ) const;

	rgba operator()( const Ray& ray, const Scene& scene, const PrimitiveHit& primitivehit, const AmbientLight& ambientlight ) const;

};