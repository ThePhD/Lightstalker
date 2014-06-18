#pragma once

#include "Math.h"
#include "Ray.h"
#include "FilmSize.h"

struct camera_look_at_t { };
const auto camera_look_at = camera_look_at_t{ };

class Camera {
private:
	vec3 referenceup;
	vec3 position, target, direction, up, right, forward;
	vec2 sensorsize;
	real focallength;
	
public:

	Camera( camera_look_at_t, vec3 pos = vec3( 0, 0, real( -100 ) ), vec3 target = vec3( 0, 0, 0 ), vec3 refup = vec3::Up, 
		real focallength = 28, vec2 framesize = FilmSize::FullFrame ) 
	: Camera( pos, pos.unnormalized_direction_to( target ), refup, focallength, framesize ) {

	}

	Camera( vec3 pos = vec3( 0, 0, real(-100) ), vec3 dir = vec3( 0, 0, real(-1) ), vec3 refup = vec3::Up, 
		real focallength = 28, vec2 framesize = FilmSize::FullFrame )
		: position( pos ), direction( dir ), referenceup( refup ), sensorsize( framesize ), focallength( focallength ) {
		target = position + dir;
		forward = normalize( direction );
		right = normalize( cross( referenceup, forward ) );
		up = normalize( cross( forward, right ) );
	}

	Ray Compute( vec2 xy, size2 widthheight ) const {
		vec2 tosensor = widthheight / sensorsize;
		tosensor /= std::max( tosensor[ 0 ], tosensor[ 1 ] );
		vec2 halfsensorsize = sensorsize / static_cast<real>( 2 );
		vec2 halfwidthheight = widthheight / static_cast<real>( 2 );
		vec2 imagexy( xy.x - halfwidthheight.x, halfwidthheight.y - xy.y );
		
		// Now translate into sensor coordinates
		// e.g. where the ray would strike on the imageplane
		vec2 normalizedxy = imagexy / halfwidthheight;
		normalizedxy *= tosensor;
		vec2 sensorxy = normalizedxy * halfsensorsize;
		
		// Compute direction based on sensor coordinates
		vec3 dir = sensorxy.x * right + sensorxy.y * up + focallength * forward;
		dir.normalize( );
		return Ray( position, dir );
	}

	Ray Compute( real x, real y, real width, real height ) const {
		return Compute( { x, y }, { width, height } );
	}

};
