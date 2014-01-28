#define FURROVINEDLL
#define _SCL_SECURE_NO_WARNINGS

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
#include <Furrovine++/Colors.h>
#include <iostream>

class Tracer {
private:
	real realwidth;
	real realheight;
	real realarea;

public:
	Tracer( real w, real h ) 
	: realwidth( w ), realheight( h ), realarea( w * h ) {
		
	}

	void Trace( real x, real y, Camera& camera, Scene& scene ) {
		Ray ray = camera.Compute( x, y, realwidth, realheight );
		auto hit = scene.Intersect( ray );                                                                                                                                                                            
		if ( !hit )
			return;
		Primitive& primitive = hit->first;
		Hit& targethit = hit->second;
		switch ( primitive.id ) {
		case PrimitiveId::Sphere:
			//std::cout << "Sphere Hit\n";
			break;
		default:
			break;
		}
	}
};

class TracerTracker {
	std::size_t width;
	std::size_t height;
	std::size_t area;
	std::vector<Fur::uint32> collision;
	std::vector<Pixel> diffuse;
	std::vector<Pixel> specular;
	std::vector<Pixel> ambient;
	Fur::buffer2d_view<Fur::uint32> collisionview;
	Fur::buffer2d_view<Pixel> diffuseview;
	Fur::buffer2d_view<Pixel> specularview;
	Fur::buffer2d_view<Pixel> ambientview;

	TracerTracker( std::size_t w, std::size_t h )
		: width( w ), height( h ), area( width * height ),
		collision( area, false ),
		diffuse( area, Pixel{ 0, 0, 0, 0 } ), specular( area, Pixel{ 0, 0, 0, 0 } ), ambient( area, Pixel{ 0, 0, 0, 0 } ),
		collisionview( collision, width, height ),
		diffuseview( diffuse, width, height ), specularview( specular, width, height ), ambientview( ambient, width, height ) {

	}
};

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;
	using namespace Furrovine::Pipeline;

	Image2D image( 800, 600, SurfaceFormat::Red32Green32Blue32Alpha32 );
	Image2D wbmp = WBMPLoader( )( "test.wbmp" );
	buffer_view<Pixel> imagepixels = image.View<Pixel>( );
	std::fill_n( imagepixels.data( ), imagepixels.size( ), Pixel{ 1.0f, 1.0f, 1.0f, 1.0f } );
	ImageOutput output( image );
	Camera camera( Vec3( 0, 0, -200 ), Vec3( 0, 0, 0 ) );
	Tracer tracer( 800, 600 );
	Scene scene;
	scene.Add( sphere_arg, 1.0f, Vec3( 0, 0, 0 ) );
	//scene.Add( plane_arg, 0.0f, Vec3::Up );
	Stopwatch stopwatch;                
	WindowDriver windowdriver;
	Window window( windowdriver, WindowDescription( "Lightstalker" ) );
	GraphicsDevice graphics( window );
	MessageQueue messagequeue;
	window.Show( );

	while ( true ) {
		windowdriver.Push( window, messagequeue );
		optional<MessageData> opmessage;
		while ( opmessage = messagequeue.pop( ) ) {
			MessageData& message = opmessage.value();
			switch ( message.header.id ) {
			case MessageId::Keyboard: 
				{ KeyboardEvent& keyboard = message.as<KeyboardEvent>( );
				if ( keyboard.Key == Key::R
					&& keyboard.Down ) {
					
				}}
				break;
			}
		}
		
		continue;
		
		for ( real y = 0; y < 600; ++y ) {
			for ( real x = 0; x < 800; ++x ) {
				tracer.Trace( x, y, camera, scene );
			}
		}
	}
}
