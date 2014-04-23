#pragma once

typedef float real;

const real real_zero = static_cast<real>( 0 );
const real real_one = static_cast<real>( 1 );
const real real_neg_one = static_cast<real>( -1 );
const real real_two = static_cast<real>( 2 );

struct RayTrace;
struct RayShader;

class Scene;
class Multisampler;
class RayTracer;
class Output;