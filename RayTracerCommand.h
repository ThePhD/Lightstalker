#pragma once

#include "Camera.h"
#include "Scene.h"
#include "RayBouncer.h"
#include "Multisampler.h"
#include <Furrovine++/optional.h>
#include <random>

struct RayTracerCommand {
	Camera camera;
	Scene scene;
	RayBouncer bouncer;
	RayShader shader;
	Fur::optional<Multisampler> multisampler;
	vec2u imagesize;
	std::size_t threadcount;
	bool multithreading;
	bool displaywindow;

	RayTracerCommand( );
};