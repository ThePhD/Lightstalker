#pragma once

#include "real.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "RayTracerCommand.hpp"
#include <Furrovine++/IO/TextReader.hpp>

struct RayTracerCommandLoader {
	RayTracerCommand operator()( const Fur::string& file );
	RayTracerCommand operator()( Fur::IO::Stream& stream );
	RayTracerCommand operator()( Fur::IO::TextReader<>& stream );
};
