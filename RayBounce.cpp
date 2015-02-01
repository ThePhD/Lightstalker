#include "RayBounce.hpp"

RayBounce::RayBounce( ) : ray( ), color( 0, 0, 0, 0 ), hit( ), shadow( false ), samples( 0 ), reflections( 0 ),
cullingtraversalhits( 0 ), primitivetests( 0 ), primitivehits( 0 ), overlappingprimitivehits( 0 ),
hitid( 0 ) {

}

void RayBounce::accumulate( const RayBounce& bounce ) {
	samples += bounce.samples;
	reflections += bounce.reflections;
	refractions += bounce.refractions;
	cullingtraversalhits += bounce.cullingtraversalhits;
	primitivetests += bounce.primitivetests;
	primitivehits += bounce.primitivehits;
	overlappingprimitivehits += bounce.overlappingprimitivehits;
	color += bounce.color;
	shadow |= bounce.shadow;
	if ( bounce.hit ) {
		hit = PrimitiveHit( *bounce.hit );
		ray = bounce.ray;
		hitid = bounce.hitid;
	}
}
