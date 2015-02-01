#pragma once

#include <Furrovine++/Enums.hpp>

enum class RayTracerStep {
	None = 0x0,
	Preliminary = 0x01,
	MultisampleDetection = 0x02,
	Multisampling = 0x04,
};

enumflagoperators( RayTracerStep );