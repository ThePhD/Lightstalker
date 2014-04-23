#include "SampleScene.h"
#include "Scene.h"
#include "Multisampler.h"
#include "Camera.h"
#include "ImageOutput.h"
#include "RayShader.h"
#include "RayTracer.h"
#include <Furrovine++/Sampling.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Stopwatch.h>
#include <iostream>

int main( ) {
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;
	
	Image2D image( 800, 600, SurfaceFormat::Red8Green8Blue8Alpha8Normalized );
	std::fill_n( image.data( ), image.size( ), 0 );
	ImageOutput output( image );
	
	//Camera camera( Vec3( 0, 30, -300 ), Vec3( 0, 0, 0 ), Vec3::Up, 500.0f );
	//Scene scene = SampleScene::SizedSpheres( );
	Camera camera( Vec3( 0, 400, -30 ), Vec3( 0, 0, 0 ), Vec3::Forward, 500.0f );
	Scene scene = SampleScene::Trifecta( );
	//Camera camera( Vec3( 0, 10, -10 ), Vec3( 0, 0, 0 ), Vec3::Up, 500.0f );
	//Scene scene = SampleScene::Complex( );

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
	std::default_random_engine randomengine{ };
	Fur::TimeSpan computationallimit = Fur::TimeSpan::FromMilliseconds( 750 );
	Multisampler multisampler( 2, 2, randomengine );
	std::vector<rgba> samples( multisampler.size( ) );
	RayShader shader;
	RayTracer tracer;
	if ( displaywindow )
		window.Show( );

	while ( true ) {
		if ( quit )
			break;
		windowdriver.Push( window, messagequeue );
		Fur::optional<Fur::MessageData> opmessage;
		while ( opmessage = messagequeue.pop( ) ) {
			Fur::MessageData& message = opmessage.value( );
			switch ( message.header.id ) {
			case Fur::MessageId::Keyboard: {
				Fur::KeyboardEvent& keyboard = message.as<Fur::KeyboardEvent>( );
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
			case Fur::MessageId::Window: {
				Fur::WindowEvent& windowm = message.as<Fur::WindowEvent>( );
				quit = windowm.Signal == Fur::WindowEventSignal::Quit
					|| windowm.Signal == Fur::WindowEventSignal::Destroy;
				}
				break;
			}
		}

		if ( quit )
			break;

		stopwatch.Start( );
		timerbreak = false;
		for ( ; y < 600 && !timerbreak; ) {
			for ( ; x < 800 && !timerbreak; ++x ) {
				samples.clear();
				rgba pixel{ };
				for ( std::size_t s = 0; s < multisampler.size( ); ++s ) {
					const Vec2& multisample = multisampler[ s ];
					real sx = x + multisample.x;
					real sy = y + multisample.y;
					Ray ray = camera.Compute( sx, sy, width, height );
					rgba sample = tracer.Bounce( ray, scene, shader );
					samples.push_back( sample );
				}
				multisampler.Shuffle( );

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

		if ( quit )
			break;

		graphics.Clear( PastelGrey );
		graphics.RenderImage( image );
		graphics.Present( );
	}
}
