#define FURROVINEDLL
#define _SCL_SECURE_NO_WARNINGS

#include "Scene.h"
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

	Ray Generate( real x, real y, const Camera& camera ) {
		return camera.Compute( x, y, realwidth, realheight );
	}

	void Evaluate( real x, real y, Scene& scene, const Camera& camera, Output& output, std::size_t depth = 0 ) {
		Ray ray = Generate( x, y, camera );
		Fur::optional<std::pair<Primitive&, Hit>> intersection = scene.Intersect( ray );
		if ( !intersection )
			return;
		const Primitive& primitive = intersection->first;
		const Hit& hit = intersection->second;
		rgba pixel = scene.Shade( ray, primitive, hit );
		output.Set( x, y, pixel );
	}
};

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;

	half h;
	half hf0 = 0.0f;
	half hf1 = 1.0f;
	half hf2 = 2.0f;
	half hf3 = 3.0f;
	half hf4 = -124.0625;
	auto vf0 = Fur::detail::crunch_half( hf0.bits( ) );
	auto vf1 = Fur::detail::crunch_half( hf1.bits( ) );
	auto vf2 = Fur::detail::crunch_half( hf2.bits( ) );
	auto vf3 = Fur::detail::crunch_half( hf3.bits( ) );
	auto vf4 = Fur::detail::crunch_half( hf4.bits( ) );
	
	Tracer tracer( 800, 600 );
	Image2D image( 800, 600, SurfaceFormat::Red8Green8Blue8Alpha8Normalized );
	std::fill_n( image.data( ), image.size( ), 0 );
	ImageOutput output( image );
	Scene scene;
	scene.Add( sphere_arg, 10.0f, Vec3( 0, 0, 0 ) );
	scene.Add( plane_arg, 0.0f, Vec3( 0, 1, 0 ) );
	scene.AddAmbientLight( 1.02f, 0.02f, 0.02f, 1.0f );
	scene.AddDirectionalLight( Vec3::Down );
	Camera camera( Vec3( 0, 0, -15 ), Vec3( 0, 0, 0 ) );

	Stopwatch stopwatch;                
	WindowDriver windowdriver;
	Window window( windowdriver, WindowDescription( "Lightstalker" ) );
	GraphicsDevice graphics( window );
	MessageQueue messagequeue;
	window.Show( );
	
	real x = 0;
	real y = 0;

	bool quit = false;
	bool timerbreak = false;
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
					// Reload with FileSystemWatcher
					x = 0;
					y = 0;
					std::fill_n( image.data( ), image.size( ), 0 );
				}
				if ( ( keyboard.Key == Key::Escape
					|| keyboard.Key == Key::Q )
					&& keyboard.Down ) {
					quit = true;
					break;
				}
				if ( keyboard.Key == Key::S
					&& keyboard.Down ) {
					output.Save( );
					break;
				}}
				break;
			}
		}

		if ( quit ) {
			break;
		}

		stopwatch.Start( );
		timerbreak = false;
		for ( ; y < 600 && !timerbreak; ) {
			for ( ; x < 800 && !timerbreak; ++x ) {
				tracer.Evaluate( x, y, scene, camera, output );
				timerbreak = stopwatch.ElapsedMilliseconds( ) > 64;
			}
			if ( x == 800 ) {
				++y;
				x = 0;
			}
		}
		
		if ( !graphics.Ready( ) ) {
			continue;
		}

		graphics.Clear( Colors::Black );
		graphics.RenderImage( image );
		graphics.Present( );
	}
}
