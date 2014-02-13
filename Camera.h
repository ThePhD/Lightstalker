#pragma once

#include "real.h"
#include "Ray.h"

class Camera {
private:
	Vec3 position, target, direction, up, right, forward;

public:

	Camera( Vec3 pos, Vec3 lookat ) : position( pos ), target( lookat ) {
		direction = target - position;
		forward = direction;
		forward.normalize( );
		right = normalize( cross( forward == Vec3::Up || -forward == Vec3::Up ? Vec3::Right : Vec3::Up, forward ) );
		up = normalize( cross( forward, right ) );
	}

	Ray Compute( real x, real y, real width, real height ) const {
		real normalizedi = ( x / width ) - static_cast<real>( 0.5 );
		real normalizedj = ( y / height ) - static_cast<real>( 0.5 );
		normalizedi *= width;
		normalizedj *= height;
		Vec3 imageplanepoint = normalizedi * right +
			normalizedj * up +
			position + direction;
		Vec3 raydir = imageplanepoint - position;
		raydir.normalize( );
		return Ray( position, raydir );
	}
};
