#pragma once

#include "real.h"
#include "Scene.h"
#include "Camera.h"
#include "RayTracerCommand.h"

struct RayTracerCommandLoader {
	RayTracerCommand operator()( const Fur::String& file );
	RayTracerCommand operator()( Fur::IO::Stream& stream );
	RayTracerCommand operator()( Fur::IO::TextReader& stream );
};
