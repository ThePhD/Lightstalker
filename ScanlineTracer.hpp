#pragma once

#include "TileTracer.hpp"

template <std::size_t n = 256>
using ScanlineTracer = TileTracer<n, 1>;
