#pragma once

#include "real.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "RayTracerCommand.hpp"
#include <Furrovine++/IO/text_reader.hpp>

struct RayTracerCommandLoader {
	RayTracerCommand operator()( const Fur::string& file );
	RayTracerCommand operator()( Fur::IO::stream& stream );
	RayTracerCommand operator()( Fur::IO::text_reader<>& stream );
};
