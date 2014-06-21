#pragma once

#include "PrimitiveHit.h"
#include "rgba.h"
#include "Light.h"
#include "RayBounce.h"

struct RayShader {
public:
	bool Shadows;

	RayShader( );

	bool Shadow( RayBounce& raybounce, const Scene& scene ) const;

	void operator()( RayBounce& raybounce, const Scene& scene ) const;

	void operator()( RayBounce& raybounce, const Scene& scene, const PointLight& pointlight ) const;

	void operator()( RayBounce& raybounce, const Scene& scene, const DirectionalLight& directionallight ) const;

	void operator()( RayBounce& raybounce, const Scene& scene, const AmbientLight& ambientlight ) const;

};