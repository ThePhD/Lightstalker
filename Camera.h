#pragma once

#include "real.h"
#include "Ray.h"

class Camera {
private:
	Vec3 position, target, direction, up, right, forward;

public:

	Camera ( Vec3 pos, Vec3 lookat ) : position( pos ), target( lookat ) {
		direction = target - position;
		forward = direction;
		forward.Normalize( );
		up = Vec3::UnitY;
		right = Vec3::Cross( forward, up );
		up = Vec3::Cross( right, forward );
	}

	Ray Compute( real x, real y, real width, real height ) {
		real normalized_i = ( x / width ) - real( 0.5 );
		real normalized_j = ( y / height ) - real( 0.5 );
		Vec3 image_point = normalized_i * right +
			normalized_j * up +
			position + direction;
		Vec3 raydir = image_point - position;
		raydir.Normalize( );
		return Ray( position, raydir );
	}
};