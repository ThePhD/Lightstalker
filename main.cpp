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
#include <Furrovine++/Sys/FileWatcher.h>
#include <Furrovine++/Stopwatch.h>
#include <Furrovine++/intersect2.h>

Furrovine::String make_info( Furrovine::Stopwatch& stopwatch, Furrovine::Graphics::Image2D& image, const vec2i& mouse, RayTracerStep steps ) {
	using namespace Furrovine;
	String stepstring = "Finished\n";
	if ( Fur::HasFlags( steps, RayTracerStep::Preliminary ) ) {
		stepstring = "Running\nTracing Rays...";
	}
	else {
		if ( Fur::HasFlags( steps, RayTracerStep::MultisampleDetection | RayTracerStep::Multisampling ) )
			stepstring = "Running\nTracing and Detecting Multisample Rays...";
		else if ( Fur::HasFlags( steps, RayTracerStep::MultisampleDetection ) )
			stepstring = "Running\nDetecting Multisample Rays...";
		else if ( Fur::HasFlags( steps, RayTracerStep::Multisampling ) )
			stepstring = "Running\nTracing Multisampling Rays...";
	}

	double elapsedmilliseconds = stopwatch.ElapsedMilliseconds( );
	int hours = static_cast<int>( elapsedmilliseconds / 1000 / 60 / 60 );
	int minutes = static_cast<int>( ( elapsedmilliseconds / 1000 / 60 ) - ( hours * 60 ) );
	int secs = static_cast<int>( ( elapsedmilliseconds / 1000 ) - ( hours * 60 * 60 ) - ( minutes * 60 ) );
	int milliseconds = static_cast<int>( (elapsedmilliseconds)-( hours * 60 * 60 * 1000 ) - ( minutes * 60 * 1000 ) - ( secs * 1000 ) );
	String hstring = lexical_cast( hours );
	String mstring = lexical_cast( minutes );
	String sstring = lexical_cast( secs );
	String msstring = lexical_cast( milliseconds );
	if ( hours < 10 )
		hstring.Prepend( "0" );
	if ( minutes < 10 )
		mstring.Prepend( "0" );
	if ( secs < 10 )
		sstring.Prepend( "0" );
	if ( milliseconds < 10 )
		msstring.Prepend( "00" );
	else if ( milliseconds < 100 )
		msstring.Prepend( "0" );

	String timestring = Format( "{0}:{1}:{2}.{3}",
		hstring, mstring, sstring, msstring );
	String datastring = Format( "{2}\nMouse - x: {0} | y: {1}\nTime - {3}",
		lexical_cast( mouse.x ), lexical_cast( mouse.y ),
		stepstring,
		timestring );
	return datastring;
}

Furrovine::String make_coords_info( Furrovine::Graphics::Image2D& image, const vec2i& coords ) {
	using namespace Furrovine;
	auto view = image.view<Fur::ByteColor>( );
	const ByteColor& color = view[ coords ];
	String coordsstring = Format( "Pixel Coordinate\nr: {2}\ng: {3}\nb: {4}",
		lexical_cast( coords.x ),
		lexical_cast( coords.y ),
		lexical_cast( static_cast<int>( color.r ) ),
		lexical_cast( static_cast<int>( color.g ) ),
		lexical_cast( static_cast<int>( color.b ) ) );
	return coordsstring;
}

template <typename TTracer>
void RayTrace( RayTracerCommand& command, Furrovine::Stopwatch& stopwatch, Furrovine::optional<const Furrovine::String&> source, Furrovine::Graphics::Image2D& image, ImageOutput& output, TTracer&& raytracer ) {
	using namespace Furrovine;
	using namespace Furrovine::Colors;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	using namespace Furrovine::Sys;
	vec2 offset = { 220, 0 };
	vec2 magoffset = { offset.x + 60, 0 };
	vec2 magsize = { 16, 16 };
	uint32 width = command.imagesize.x;
	uint32 height = command.imagesize.y + 55;
	real swidth = static_cast<real>( command.imagesize.x );
	real sheight = static_cast<real>( command.imagesize.y );
	WindowDriver windowdriver( Fur::WindowDriverFlags::Default );
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", Fur::Size2u32( width, height ) ) );
	GraphicsDevice graphics( window );
	NymphBatch batch( graphics );
	MessageQueue messagequeue;
	RasterFont font = RasterFontLoader( graphics )( RasterFontDescription( "Arial", 11 ) );
	KeyboardDevice keyboard( 0 );
	vec2i mousepos( 0, 0 );
	optional<vec2i> checkcoord = nullopt;

	FileWatcher watcher( maybe_or( source, String( "" ) ), "", false );
	
	std::vector<byte> watcherbuffer( 2048 * 4 );
	std::vector<FileDelta> changes;
	changes.reserve( 4 );

	bool quit = false;
	bool autosaved = false;
	bool doreset = false;
	bool doreload = false;
	bool& displaywindow = command.displaywindow;
	if ( displaywindow ) {
		window.Show( );
	}

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
				if ( !mouse.InWindow )
					break;
				mousepos = mouse.Relative;
				if ( AnyFlags( mouse.Buttons, MouseButtons::Left ) ) {
					checkcoord = mousepos;
				}
				if ( AnyFlags( mouse.Buttons, MouseButtons::Right ) ) {
					checkcoord = nullopt;
				}
				break; }
			case Fur::MessageId::Window: {
				Fur::WindowEvent& windowm = message.as<Fur::WindowEvent>( );
				quit = windowm.Signal == Fur::WindowEventSignal::Quit
					|| windowm.Signal == Fur::WindowEventSignal::Destroy;
				break; }
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
				doreset = true;
				doreload = static_cast<bool>( source );
			}
			if ( keyboard.Pressed( Key::S ) ) {
				output.Save( );
			}
		}

		if ( source ) {
			watcher.WaitForChanges( changes, watcherbuffer, Timeout( 33 ) );
			if ( changes.size( ) > 0 ) {
				for ( auto& change : changes ) {
					if ( change.TimedOut )
						continue;
					if ( AnyFlags( change.Changes, FileChangeFlags::Modified | FileChangeFlags::Created )
						|| ( AnyFlags( change.Changes, FileChangeFlags::Renamed ) && change.Name == *source ) ) {
						doreset = true;
						doreload = true;
						break;
					}
				}
				changes.clear( );
			}
		}

		if ( doreset ) {
			stopwatch.Stop( );
			raytracer.Stop( );
			output.Clear( );
			if ( doreload ) {
				vec2u imagesize = command.imagesize;
				command = std::move( RayTracerCommandLoader( )( *source ) );
				command.imagesize = imagesize;
				command.scene.Build( );
				doreload = false;
			}
			stopwatch.Start( );
			raytracer.Reset( );
			doreset = false;
			continue;
		}

		raytracer.Compute( );

		if ( raytracer.Check( ) ) {
			if ( !autosaved ) {
				output.Save( );
				autosaved = true;
				if ( !displaywindow ) {
					quit = true;
					continue;
				}
			}
			stopwatch.Stop( );
		}

		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

		bool rendercoord = checkcoord && intersect( image.boundaries( ), TVector2<uint32>( *checkcoord ) );
		auto imageview = image.view<ByteColor>( );
		
		graphics.Clear( Black );
		graphics.RenderImage( image, Region( 0, 0, swidth, sheight ) );
		batch.Begin( );
		String datastring = make_info( stopwatch, image, mousepos, raytracer.Steps() );
		batch.RenderString( font, datastring, { 0, sheight } );
		if ( rendercoord ) {
			auto color = imageview[ *checkcoord ];
			auto backcolor = color == White ? AmbientGrey : White;
			batch.RenderGradient( Region( offset.x + 33, sheight + 14, 32, 32 ), backcolor, backcolor );
			batch.RenderGradient( Region( offset.x + 34, sheight + 15, 30, 30 ), color, color );
			
			String coordsstring = make_coords_info( image, *checkcoord );
			batch.RenderString( font, coordsstring, { offset.x, sheight } );
		}
		else {
			batch.RenderString( font, "Click on a pixel...", { offset.x, sheight } );
		}
		batch.End( );
		graphics.Present( );
	}
}

#include <iostream>

int main( int argc, char* argv[] ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Threading;
	std::vector<String> arguments{ };
	arguments.reserve( argc );
	std::for_each( argv, argv + argc, [ &arguments ] ( const char* p ) { 
		arguments.emplace_back( buffer_view<const char>( p, std::char_traits<char>::length( p ) ) ); 
	} );

	optional<String> source = nullopt;
	optional<String> output = nullopt;
	optional<RayTracerCommand> ocommand = Fur::nullopt;
	if ( arguments.size( ) > 1 ) {
		source = arguments[ 1 ];
		if ( arguments.size( ) > 2 ) {
			output = arguments[ 2 ];
		}
	}
	try {
		if ( !output )
			output = "output.png";
		if ( source )
			ocommand = RayTracerCommandLoader()( *source );
		if (!ocommand)
			ocommand = SampleCommands::MultiSphere( );
		Stopwatch stopwatch;
		RayTracerCommand& command = *ocommand;
		Image2D image( command.imagesize, SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
		ImageOutput output( image, *output );
		Scene& scene = command.scene;
		Camera& camera = command.camera;
		RayBouncer& bouncer = command.bouncer;
		RayShader& shader = command.shader;
		optional<Multisampler>& multisampler = command.multisampler;
		vec2u& imagesize = command.imagesize;
		scene.Build( );
		if ( command.multithreading ) {
			ThreadPool threadpool( command.threadcount );
			ThreadedTileTracer<16, 16> raytracer( threadpool, imagesize, camera, 
				scene, bouncer, shader, multisampler, output );
			stopwatch.Start( );
			RayTrace( command, stopwatch, source, image, output, raytracer );
		}
		else {
			TileTracer<16, 16> raytracer( imagesize, camera,
				scene, bouncer, shader, multisampler, output, std::chrono::milliseconds( 1500 ) );
			stopwatch.Start( );
			RayTrace( command, stopwatch, source, image, output, raytracer );
		}
	}
	catch ( const Exception& ex ) {
		std::cout << "Lightstalker - An unhandled Furrovine Exception has occured:\n\t"
			<< ex.what( );
		return 2;
	}
	catch ( const std::exception& ex ) {
		std::cout << "Lightstalker - An unhandled standard exception has occured:\n\t"
			<< ex.what( );
		return 2;
	}
	catch ( ... ) {
		std::cout << "Lightstalker - A completely non-understandable exception has occurred.";
		return 1;
	}
	std::cout << "Lightstalker - Completed without exceptions." << std::endl;
	return 0;
}
