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
	bool multithreading;
	bool displaywindow;

	RayTracerCommand( ) : multisampler( Fur::in_place, 4, 4, std::default_random_engine() ), imagesize(800, 600), multithreading( true ), displaywindow( true ) {

	}
};