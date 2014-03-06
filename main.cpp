#define FURROVINEDLL
#define _SCL_SECURE_NO_WARNINGS

#include "Scene.h"
#include "Sample.h"
#include "Camera.h"
#include "ImageOutput.h"
#include <Furrovine++/Sampling.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Stopwatch.h>
#include <Furrovine++/Colors.h>
#include <iostream>

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;

	Image2D image( 800, 600, SurfaceFormat::Red8Green8Blue8Alpha8Normalized );
	std::fill_n( image.data( ), image.size( ), 0 );
	ImageOutput output( image );
	Scene scene;
	scene.Add( Material{ Colors::Red, Colors::White, 32, Colors::Transparent, rgba( 0.5f, 0.5f, 0.5f, 1.0f ) },
		sphere_arg, 50.0f, Vec3( 0, 60, 0 ) );
	scene.Add( Material{ Colors::BlueBell, Colors::Transparent, 0, Colors::Transparent, Colors::Transparent }, 
		plane_arg, -1.0f, Vec3::Up );
	scene.AddAmbientLight( 0.04f, 0.04f, 0.04f, 1.0f );
	scene.AddDirectionalLight( normalize( Vec3( 0, -1, 0 ) ) );
	Camera camera( Vec3( 0, 30, -300 ), Vec3( 0, 0, 0), 500.0f );

	Stopwatch stopwatch;                
	WindowDriver windowdriver;
	Window window( windowdriver, WindowDescription( "Lightstalker", Size2ui( 800, 600 ) ) );
	GraphicsDevice graphics( window );
	MessageQueue messagequeue;

	real x = 0;
	real y = 0;
	real width = 800;
	real height = 600;

	bool quit = false;
	bool timerbreak = false;
	bool displaywindow = true;
	TimeSpan computationallimit = TimeSpan::FromMilliseconds( 500 );
	
	std::vector<Vec2> multisamples = Sampling::grid<real>( 2, 2 );
	std::vector<std::pair<Primitive&, Hit>> hits;
	hits.reserve( 1024 );

	if ( displaywindow )
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
		std::vector<rgba> samples( 4 );
		for ( ; y < 600 && !timerbreak; ) {
			for ( ; x < 800 && !timerbreak; ++x ) {
				samples.clear();
				rgba pixel{ };
				Ray ray = camera.Compute( x, y, width, height );
				Fur::optional<std::pair<Primitive&, Hit>> intersection = scene.Intersect( ray, hits );
				if ( !intersection )
					continue;
				const Primitive& primitive = intersection->first;
				const Hit& hit = intersection->second;
				samples.push_back( scene.Shade( ray, primitive, hit, hits ) );
				
				for ( std::size_t s = 0; s < samples.size( ); ++s ) {
					pixel += samples[ s ];
				}
				
				pixel /= static_cast<real>( samples.size( ) );
				output.Set( x, y, pixel );
				
				timerbreak = ( stopwatch.ElapsedTime() > computationallimit ) && displaywindow;
			}
			if ( x == 800 ) {
				++y;
				x = 0;
			}
		}
		
		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

		graphics.Clear( Colors::PastelGrey );
		graphics.RenderImage( image );
		graphics.Present( );
	}
}
