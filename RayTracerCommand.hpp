#pragma once

#include "Camera.hpp"
#include "Scene.hpp"
#include "RayBouncer.hpp"
#include "Multisampler.hpp"
#include <Furrovine++/optional.hpp>
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