#pragma once

#include "real.h"
#include "Ray.h"

class Camera {
private:
	vec3 referenceup;
	vec3 position, target, direction, up, right, forward;
	real nearplane;
	
public:

	Camera( vec3 pos, vec3 lookat, vec3 referenceup = vec3::Up, float nearplane = 1.0f )
	: position( pos ), target( lookat ), referenceup( referenceup ), nearplane( nearplane ) {
		direction = position.unnormalized_direction_to( target );
		forward = normalize( direction );
		right = normalize( cross( referenceup, forward ) );
		up = normalize( cross( forward, right ) );
	}

	Ray Compute( vec2 xy, size2 widthheight ) const {
		return Compute( xy.x, xy.y, widthheight.x, widthheight.y );
	}

	Ray Compute( real x, real y, real width, real height ) const {
		real halfwidth = ( width / static_cast<real>( 2 ) );
		real halfheight = ( height / static_cast<real>( 2 ) );
		x = x - halfwidth;
		y = halfheight - y;
		vec3 dir = x * right + y * up + nearplane * forward;
		dir.normalize( );
		return Ray( position, dir );
	}

};
