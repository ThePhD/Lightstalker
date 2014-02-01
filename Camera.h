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
		right = Vec3::Normalize( Vec3::Cross( forward == Vec3::Up || -forward == Vec3::Up ? Vec3::Right : Vec3::Up, forward ) );
		up = Vec3::Normalize( Vec3::Cross( forward, right ) );
	}

	Ray Compute( real x, real y, real width, real height ) const {
		real normalized_i = ( x / width ) - static_cast<real>( 0.5 );
		real normalized_j = ( y / height ) - static_cast<real>( 0.5 );
		normalized_i *= width;
		normalized_j *= height;
		Vec3 image_point = normalized_i * right +
			normalized_j * up +
			position + direction;
		Vec3 raydir = image_point - position;
		raydir.Normalize( );
		return Ray( position, raydir );
	}
};