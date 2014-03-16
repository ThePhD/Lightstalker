#define FURROVINEDLL
#define _SCL_SECURE_NO_WARNINGS

#include "Scene.h"
#include "Sample.h"
#include "Camera.h"
#include "ImageOutput.h"
#include "RayShader.h"
#include "Trace.h"
#include <Furrovine++/Sampling.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Stopwatch.h>
#include <Furrovine++/Colors.h>
#include <iostream>

int main( ) {
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;
	
	Image2D image( 800, 600, SurfaceFormat::Red8Green8Blue8Alpha8Normalized );
	std::fill_n( image.data( ), image.size( ), 0 );
	ImageOutput output( image );
	Scene scene;
	scene.Add( Material{ Red, White, 32, Transparent, RealRgba( 0.5f, 0.5f, 0.5f, 1.0f ) },
		sphere_arg, 50.0f, Vec3( 0, 60, 0 ) );
	scene.Add( Material{ BlueBell, Transparent, 0, Transparent, Transparent }, 
		plane_arg, -1.0f, Vec3::Up );
	scene.AddAmbientLight( 0.04f, 0.04f, 0.04f, 1.0f );
	scene.AddDirectionalLight( normalize( Vec3( 0, -1, 0 ) ) );
	scene.AddDirectionalLight( normalize( Vec3( -1, -1, 0 ) ) );
	Camera camera( Vec3( 0, 30, -300 ), Vec3( 0, 0, 0 ), 500.0f );

	Fur::Stopwatch stopwatch;
	Fur::WindowDriver windowdriver;
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", Fur::Size2ui( 800, 600 ) ) );
	GraphicsDevice graphics( window );
	Fur::MessageQueue messagequeue;

	real x = 0;
	real y = 0;
	real width = 800;
	real height = 600;

	bool quit = false;
	bool timerbreak = false;
	bool displaywindow = true;
	Fur::TimeSpan computationallimit = Fur::TimeSpan::FromMilliseconds( 500 );
	RayShader shader;
	Trace trace, shadowtrace;
	trace.hits.reserve( 1024 );
	shadowtrace.hits.reserve( 1024 );
	std::vector<Vec2> multisamples = Fur::Sampling::grid<real>( 2, 2 );
	
	if ( displaywindow )
		window.Show( );

	while ( true ) {
		windowdriver.Push( window, messagequeue );
		Fur::optional<Fur::MessageData> opmessage;
		while ( opmessage = messagequeue.pop( ) ) {
			Fur::MessageData& message = opmessage.value( );
			switch ( message.header.id ) {
			case Fur::MessageId::Keyboard:
			{ Fur::KeyboardEvent& keyboard = message.as<Fur::KeyboardEvent>( );
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
		std::vector<RealRgba> samples( 4 );
		for ( ; y < 600 && !timerbreak; ) {
			for ( ; x < 800 && !timerbreak; ++x ) {
				samples.clear();
				RealRgba pixel{ };
				Ray ray = camera.Compute( x, y, width, height );
				scene.Intersect( ray, trace );
				if ( !trace.closesthit )
					continue;
				
				samples.push_back( scene.Lighting( ray, shader, trace, shadowtrace ) );
				
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

		graphics.Clear( PastelGrey );
		graphics.RenderImage( image );
		graphics.Present( );
	}
}
