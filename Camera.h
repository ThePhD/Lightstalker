#pragma once

#include "real.h"
#include "Ray.h"

class Camera {
private:
	Vec3 referenceup;
	Vec3 position, target, direction, up, right, forward;
	real nearplane;
	
public:

	Camera( Vec3 pos, Vec3 lookat, float nearplane = 1.0f, Vec3 referenceup = Vec3::Up ) 
	: position( pos ), target( lookat ), referenceup( referenceup ), nearplane( nearplane ) {
		direction = position.unnormalized_direction_to( target );
		forward = normalize( direction );
		right = normalize( cross( referenceup, forward ) );
		up = normalize( cross( forward, right ) );
	}

	Ray Compute( real x, real y, real width, real height ) const {
		real halfwidth = ( width / static_cast<real>( 2 ) );
		real halfheight = ( height / static_cast<real>( 2 ) );
		x = x - halfwidth;
		y = halfheight - y;
		Vec3 dir = x * right + y * up + nearplane * forward;
		dir.normalize( );
		return Ray( position, dir );
	}

};
