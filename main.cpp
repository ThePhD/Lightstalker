#include "SampleScene.h"
#include "Scene.h"
#include "Multisampler.h"
#include "Camera.h"
#include "ImageOutput.h"
#include "RayShader.h"
#include "RayTracer.h"
#include "ThreadedTileTracer.h"
#include "TileTracer.h"
#include <Furrovine++/Stopwatch.h>
#include <Furrovine++/lexical_cast.h>
#include <Furrovine++/Sampling.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/NymphBatch.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Pipeline/RasterFontLoader.h>
#include <Furrovine++/Pipeline/ImageLoader.h>
#include <Furrovine++/Graphics/Filtering/SobelEdge.h>
#include <Furrovine++/Graphics/Filtering/LaplacianEdge.h>
#include <Furrovine++/Graphics/Filtering/MotionBlur.h>
#include <Furrovine++/Graphics/Filtering/BoxBlur.h>
#include <Furrovine++/Graphics/Filtering/Greyscale.h>
#include <Furrovine++/Graphics/Filtering/GaussianBlur.h>
#include <iostream>

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Graphics::Filtering;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	
	std::size_t width = 800;
	std::size_t height = 600;
	real swidth = static_cast<real>( width );
	real sheight = static_cast<real>( height );
	Image2D image( bounds<2>( width, height ), SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
	ImageOutput output( image );
	std::default_random_engine randomengine{ };
	Multisampler multisampler( 4, 4, randomengine );
	RayShader shader;
	RayTracer tracer;
	//Camera camera( vec3( 0, 30, -300 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	//Scene scene = SampleScene::SizedSpheres( );
	Camera camera( vec3( 0, 0, -500 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	Scene scene = SampleScene::SimpleObjScene( );
	//Camera camera( vec3( 0, 10, -10 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
	//Scene scene = SampleScene::Complex( );
	TileTracer<16, 16> tiletracer( width, height, camera, scene, tracer, shader, multisampler, output, std::chrono::milliseconds( 750 ) );

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

		tiletracer.Compute( );
		
		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

		if ( quit )
			break;

		graphics.Clear( Black );
		graphics.RenderImage( image, Region( 0, 0, swidth, sheight ) );
		batch.Begin( );
		String datastring = Format( "[ x: {0} | y: {1} ]", lexical_cast( mousex ), lexical_cast( mousey ) );
		batch.RenderString( font, datastring, { 0, sheight } );
		batch.End( );
		graphics.Present( );
	}
}
