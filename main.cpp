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
#include <Furrovine++/Input/MouseDevice.h>
#include <Furrovine++/Input/KeyboardDevice.h>
#include <Furrovine++/Graphics/Window.h>
#include <Furrovine++/Graphics/GraphicsDevice.h>
#include <Furrovine++/Graphics/NymphBatch.h>
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/Pipeline/RasterFontLoader.h>
#include <Furrovine++/Sys/FileWatcher.h>
#include <Furrovine++/Stopwatch.h>
#include <Furrovine++/intersect2.h>

Furrovine::String make_coords_info( Furrovine::Graphics::Image2D& image, const vec2i& coords ) {
	using namespace Furrovine;
	TVector2<uint32> ucoords( coords );
	if ( !intersect( image.boundaries( ), ucoords ) ) {
		return "Mouse over pixel...";
	}
	const Furrovine::ByteColor& color = image.view<ByteColor>()[ coords ];
	String coordsstring = Format( "Pixel - r: {0} | g: {1} | b: {2}",
		lexical_cast( static_cast<int>( color.r ) ),
		lexical_cast( static_cast<int>( color.g ) ),
		lexical_cast( static_cast<int>( color.b ) ) );
	return coordsstring;
}

Furrovine::String make_info( Furrovine::Stopwatch& stopwatch, Furrovine::Graphics::Image2D& image, const vec2i& mouse, RayTracerStep steps ) {
	using namespace Furrovine;
	String stepstring = "Finished\n";
	if ( Fur::HasFlags( steps, RayTracerStep::Preliminary ) ) {
		stepstring = "Running\nTracing Rays...";
	}
	else {
		if ( Fur::HasFlags( steps, RayTracerStep::MultisampleDetection | RayTracerStep::Multisampling ) )
			stepstring = "Running\nDetecting and Tracing Multisample Rays...";
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
	String datastring = Format( "{0}\nMouse - x: {1} | y: {2}\n{3}\nTime - {4}",
		stepstring,
		lexical_cast( mouse.x ), lexical_cast( mouse.y ),
		make_coords_info( image, mouse ),
		timestring );
	return datastring;
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
	uint32 bottombar = 72;
	vec2 imagesize( static_cast<real>( command.imagesize.x ), static_cast<real>( command.imagesize.y ) );
	Fur::Size2u32 windowsize( command.imagesize.x, command.imagesize.y + bottombar );
	real offsetx = 220;
	
	vec2u magtoolsize( 32, 32 );
	vec2u magsize( 16, 16 );
	vec2 magpixelsize( 4, 4 );
	vec2 magoffset( offsetx + 60, imagesize.y + ( static_cast<real>(bottombar)-( magsize.y * magpixelsize.y ) ) / 2 );

	WindowDriver windowdriver( Fur::WindowDriverFlags::Default );
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", windowsize ) );
	GraphicsDevice graphics( window );
	NymphBatch batch( graphics );
	MessageQueue messagequeue;
	RasterFont font = RasterFontLoader( graphics )( RasterFontDescription( "Arial", 11 ) );
	KeyboardDevice keyboard( 0 );
	MouseDevice mouse( 0 );
	vec2i mousepos( 0, 0 );

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
			case Fur::MessageId::Window: {
				Fur::WindowEvent& windowm = message.as<Fur::WindowEvent>( );
				quit = windowm.Signal == Fur::WindowEventSignal::Quit
					|| windowm.Signal == Fur::WindowEventSignal::Destroy;
				break; }
			}
		}

		if ( quit )
			break;
		
		mouse.Update( );
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
		mousepos = mouse.Position( );

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

		auto imageview = image.view<ByteColor>( );
		
		graphics.Clear( Black );
		graphics.RenderImage( image, Region( 0, 0, imagesize.x, imagesize.y ) );
		batch.Begin( );
		String datastring = make_info( stopwatch, image, mousepos, raytracer.Steps() );
		batch.RenderString( font, datastring, { 0, imagesize.y } );
		auto backcolor = AmbientGrey;
		
		if ( mouse.Down( MouseButtons::Right ) ) {
			vec2 backpos( mousepos );
			backpos -= magpixelsize * static_cast<real>( 12 );
			backpos -= static_cast<real>( 1 );
			batch.RenderGradient( Region( backpos, size2( vec2( magtoolsize ) * magpixelsize + static_cast<real>( 2 ) ) ), backcolor );
			for ( std::size_t y = 0; y < magtoolsize.x; ++y ) {
				for ( std::size_t x = 0; x < magtoolsize.y; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					vec2u pos( mousepos.x + x, mousepos.y + y );
					pos -= magtoolsize / static_cast<std::size_t>( 2 );
					auto color = !intersect( image.boundaries( ), pos ) ? Black : imageview[ pos ];
					color.a = 255;
					vec2 renderpos( static_cast<real>( mousepos.x ), static_cast<real>( mousepos.y ) );
					renderpos -= magpixelsize * static_cast<real>( 12 );
					renderpos += magpixelsize * vec2( sx, sy );
					Region region( renderpos, size2( magpixelsize ) );
					batch.RenderGradient( region, color );
				}
			}
		}
		else {
			batch.RenderGradient( Region( magoffset - vec2( 1, 1 ), size2( vec2( magsize ) * magpixelsize + vec2( 2, 2 ) ) ), backcolor );
			for ( std::size_t y = 0; y < magsize.y; ++y ) {
				for ( std::size_t x = 0; x < magsize.x; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					vec2u pos( mousepos.x + x, mousepos.y + y );
					pos -= magsize / static_cast<std::size_t>( 2 );
					auto color = !intersect( image.boundaries( ), pos ) ? Black : imageview[ pos ];
					color.a = 255;
					vec2 renderpos( magoffset );
					renderpos += magpixelsize * vec2( sx, sy );
					Region region( renderpos, size2( magpixelsize ) );
					batch.RenderGradient( region, color );
				}
			}
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
		if ( !output ) {
			if ( source ) {
				output = *source + String( ".png" );
			}
			else {
				output = "output.png";
			}
		}
		if ( source )
			ocommand = RayTracerCommandLoader()( *source );
		if (!ocommand)
			ocommand = SampleCommands::MultiSphere( );
		Stopwatch stopwatch;
		RayTracerCommand& command = *ocommand;
		Image2D image( command.imagesize, SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
		ImageOutput imageoutput( image, *output );
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
				scene, bouncer, shader, multisampler, imageoutput );
			stopwatch.Start( );
			RayTrace( command, stopwatch, source, image, imageoutput, raytracer );
		}
		else {
			TileTracer<16, 16> raytracer( imagesize, camera,
				scene, bouncer, shader, multisampler, imageoutput, std::chrono::milliseconds( 1500 ) );
			stopwatch.Start( );
			RayTrace( command, stopwatch, source, image, imageoutput, raytracer );
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
