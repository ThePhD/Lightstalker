#include "SampleCommands.h"
#include "Scene.h"
#include "Multisampler.h"
#include "Camera.h"
#include "ImageOutput.h"
#include "RayShader.h"
#include "RayBouncer.h"
#include "RayTracerCommand.h"
#include "TileTracer.h"
#include "ScanlineTracer.h"
#include "ThreadedTileTracer.h"
#include "RayTracerCommandLoader.h"
#include <Furrovine++/lexical_cast.h>
#include <Furrovine++/WindowDriver.h>
#include <Furrovine++/Input/KeyboardDevice.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/NymphBatch.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Pipeline/RasterFontLoader.h>

template <typename TTracer>
void RayTrace( RayTracerCommand& command, Furrovine::Graphics::Image2D& image, ImageOutput& output, TTracer&& raytracer ) {
	using namespace Furrovine;
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	Fur::WindowDriver windowdriver( Fur::WindowDriverFlags::Default );
	Fur::uint32 width = command.imagesize.x;
	Fur::uint32 height = command.imagesize.y;
	real swidth = static_cast<real>( command.imagesize.x );
	real sheight = static_cast<real>( command.imagesize.y );
	height += 60;
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", Fur::Size2u32( width, height ) ) );
	GraphicsDevice graphics( window );
	NymphBatch batch( graphics );
	Fur::MessageQueue messagequeue;
	RasterFont font = RasterFontLoader( graphics )( RasterFontDescription( "Arial", 14 ) );
	KeyboardDevice keyboard( 0 );
	int mousex = 0;
	int mousey = 0;

	bool quit = false;
	bool autosave = false;
	bool& displaywindow = command.displaywindow;
	if ( displaywindow )
		window.Show( );

	while ( true ) {
		if ( quit ) {
			windowdriver.Quit( window );
		}
		windowdriver.Push( window, messagequeue );
		Fur::optional<Fur::MessageData> opmessage;
		while ( opmessage = messagequeue.pop( ) ) {
			Fur::MessageData& message = opmessage.value( );
			switch ( message.header.id ) {
			case Fur::MessageId::Mouse: {
				Fur::MouseEvent& mouse = message.as<Fur::MouseEvent>( );
				mousex = mouse.Relative.x;
				mousey = mouse.Relative.y;
				} break;
			case Fur::MessageId::Window: {
				Fur::WindowEvent& windowm = message.as<Fur::WindowEvent>( );
				quit = windowm.Signal == Fur::WindowEventSignal::Quit
					|| windowm.Signal == Fur::WindowEventSignal::Destroy;
				} break;
			}
		}

		if ( quit )
			break;
		
		keyboard.Update( );
		if ( keyboard.Pressed( Key::Escape ) ) {
			quit = true;
		}
		if ( keyboard.ControlDown() ) {
			if ( keyboard.Pressed( Key::Q ) ) {
				quit = true;
			}
			if ( keyboard.Pressed( Key::R ) ) {
				// Reload with FileSystemWatcher
				raytracer.Stop( );
				output.Clear( );
				raytracer.Reset( );
			}
			if ( keyboard.Pressed( Key::S ) ) {
				output.Save( );
			}
		}

		raytracer.Compute( );

		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

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

#include <iostream>

int main( ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Threading;
	try {
		RayTracerCommand command = SampleCommands::MultiSphere( );
		Image2D image( command.imagesize, SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
		ImageOutput output( image );
		Scene& scene = command.scene;
		Camera& camera = command.camera;
		RayBouncer& bouncer = command.bouncer;
		RayShader& shader = command.shader;
		optional<Multisampler>& multisampler = command.multisampler;
		vec2u& imagesize = command.imagesize;
		if ( command.multithreading ) {
			ThreadPool threadpool{ };
			ThreadedTileTracer<16, 16> raytracer( threadpool, imagesize, camera, 
				scene, bouncer, shader, multisampler, output );
			RayTrace( command, image, output, raytracer );
		}
		else {
			TileTracer<16, 16> raytracer( imagesize, camera,
				scene, bouncer, shader, multisampler, output, std::chrono::milliseconds( 1500 ) );
			RayTrace( command, image, output, raytracer );
		}
	}
	catch ( Exception& ex ) {
		std::cout << "An unhandled Furrovine Exception has occured:\n\t"
			<< ex.what( );
		return 2;
	}
	catch ( std::exception& ex ) {
		std::cout << "An unhandled standard exception has occured:\n\t"
			<< ex.what( );
		return 2;
	}
	catch ( ... ) {
		std::cout << "A completely non-understandable exception has occurred.";
		return 1;
	}
	return 0;
}
