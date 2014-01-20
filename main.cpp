#define FURROVINEDLL

#include "Scene.h"
#include "Pixel.h"
#include "Sample.h"
#include "Camera.h"
#include "ImageOutput.h"
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/buffer2d_view.h>
#include <Furrovine++/Stopwatch.h>
#include <iostream>

class Lightstalker {
private:
	std::size_t width;
	std::size_t height;
	std::size_t area;
	real realwidth;
	real realheight;
	real realarea;
	std::vector<Fur::uint32> collision;
	std::vector<Pixel> diffuse;
	std::vector<Pixel> specular;
	std::vector<Pixel> ambient;
	Fur::buffer2d_view<Fur::uint32> collisionview;
	Fur::buffer2d_view<Pixel> diffuseview;
	Fur::buffer2d_view<Pixel> specularview;
	Fur::buffer2d_view<Pixel> ambientview;

public:
	Lightstalker( std::size_t w, std::size_t h ) 
	: width( w ), height( h ), area( width * height ),
	realwidth( static_cast<real>( width ) ), realheight( static_cast<real>( height ) ), realarea( static_cast<real>( area ) ),
	collision( area, false ),
	diffuse( area, Pixel{ 0, 0, 0, 0 } ), specular( area, Pixel{ 0, 0, 0, 0 } ), ambient( area, Pixel{ 0, 0, 0, 0 } ),
	collisionview( collision, width, height ),
	diffuseview( diffuse, width, height ), specularview( specular, width, height ), ambientview( ambient, width, height ) {
		
	}

	void Trace( real x, real y, Camera& camera, Scene& scene ) {
		Ray ray = camera.Compute( x, y, realwidth, realheight );
		Ray ray2( Vec3( 0, 0, -20 ), Vec3( 0, 0, 1 ) );
		auto hit = scene.Intersect( ray2 );
		if ( !hit )
			return;
		Primitive& primitive = hit->first;
		switch ( primitive.id ) {
		case PrimitiveId::Sphere:
			//std::cout << "Sphere Hit\n";
			break;
		default:
			break;
		}
	}
};

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;

	Image2D image( 800, 600, SurfaceFormat::Red32Green32Blue32Alpha32 );
	ImageOutput output( image );
	Camera camera( Vec3( 0, 0, -20 ), Vec3( 0, 0, 0 ) );
	Scene scene;
	scene.Add( sphere_arg, 1.0f, Vec3( 0, 0, 0 ) );
	
	Lightstalker raytracer( 800, 600 );
	
	Stopwatch stopwatch;
	WindowDriver windowdriver;
	Window window( windowdriver, WindowDescription( "Lightstalker" ) );
	GraphicsDevice graphics( window );

	for ( real y = 0; y < 600; ++y ) {
		for ( real x = 0; x < 800; ++x ) {
			raytracer.Trace( x, y, camera, scene );
		}
	}
}
