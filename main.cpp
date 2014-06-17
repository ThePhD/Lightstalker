#include "SampleScene.h"
#include "Scene.h"
#include "Multisampler.h"
#include "Camera.h"
#include "ImageOutput.h"
#include "RayShader.h"
#include "RayBouncer.h"
#include "TileTracer.h"
#include "ScanlineTracer.h"
#include "ThreadedTileTracer.h"
#include <Furrovine++/lexical_cast.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/NymphBatch.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Pipeline/RasterFontLoader.h>

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	using namespace Furrovine::Threading;

	ThreadPool threadpool;

	std::size_t width = 800;
	std::size_t height = 600;
	real swidth = static_cast<real>( width );
	real sheight = static_cast<real>( height );
	Image2D image( bounds<2>( width, height ), SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
	ImageOutput output( image );
	std::default_random_engine randomengine{ };
	Multisampler multisampler( 4, 4, randomengine );
	RayShader shader{ };
	RayBouncer bouncer{ };
	//Scene scene = SampleScene::Trifecta( );
	//Camera camera( vec3( 0, 500, -500 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	Scene scene = SampleScene::SizedSpheres( );
	Camera camera( vec3( 0, 30, -300 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	//Scene scene = SampleScene::SimpleObjScene( );
	//Camera camera( vec3( 0, 0, -500 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	//Scene scene = SampleScene::ObjScene( );
	//Camera camera( vec3( 3, 3, 3 ) * scene.Bounds().max, vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	//Scene scene = SampleScene::Complex( );
	//Camera camera( vec3( 0, 10, -10 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	
	//ScanlineTracer<256> tiletracer( { width, height }, camera, scene, tracer, shader, multisampler, output, std::chrono::milliseconds( 750 ) );
	//TileTracer<16, 16> tiletracer( { width, height }, camera, scene, tracer, shader, multisampler, output, std::chrono::milliseconds( 750 ) );
	ThreadedTileTracer<16, 16> raytracer( threadpool, { width, height }, camera, scene, bouncer, shader, multisampler, output );

	Fur::WindowDriver windowdriver;
	height += 60;
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", Fur::Size2u32( width, height ) ) );
	GraphicsDevice graphics( window );
	NymphBatch batch( graphics );
	Fur::MessageQueue messagequeue;
	RasterFont font = RasterFontLoader( graphics )( RasterFontDescription( "Arial", 14 ) );
	real x = 0;
	real y = 0;
	int mousex = 0;
	int mousey = 0;
	
	bool quit = false;
	bool displaywindow = true;
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
					std::fill_n( image.raw_view( ).data(), image.raw_view( ).size( ), 0 );
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
			case Fur::MessageId::Mouse: {
				Fur::MouseEvent& mouse = message.as<Fur::MouseEvent>( );
				mousex = mouse.Relative.x;
				mousey = mouse.Relative.y;
				}
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

		raytracer.Compute( );
		
		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

		if ( quit )
			break;

		RayTracerStep steps = raytracer.Steps( );
		String stepstring = "Finished";
		if ( Fur::HasFlags( steps, RayTracerStep::Preliminary ) )
			stepstring = "Running - Tracing Rays...";
		else if ( Fur::HasFlags( steps, RayTracerStep::MultisampleDetection ) )
			stepstring = "Running - Detecting Multisample Rays...";
		else if ( Fur::HasFlags( steps, RayTracerStep::Multisampling ) )
			stepstring = "Running - Tracing Multisampling Rays...";
		String datastring = Format( "[ x: {0} | y: {1} ] - {2}",
			lexical_cast( mousex ), lexical_cast( mousey ),
			stepstring );
		
		graphics.Clear( Black );
		graphics.RenderImage( image, Region( 0, 0, swidth, sheight ) );
		batch.Begin( );
		batch.RenderString( font, datastring, { 0, sheight } );
		batch.End( );
		graphics.Present( );
	}
}
