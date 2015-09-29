#include "SampleCommands.hpp"
#include "Scene.hpp"
#include "Multisampler.hpp"
#include "Camera.hpp"
#include "ImageOutput.hpp"
#include "RayShader.hpp"
#include "RayBouncer.hpp"
#include "RayTracerCommand.hpp"
#include "TileTracer.hpp"
#include "ScanlineTracer.hpp"
#include "ThreadedTileTracer.hpp"
#include "RayTracerCommandLoader.hpp"
#include <Furrovine++/lexical_cast.hpp>
#include <Furrovine++/queue.hpp>
#include <Furrovine++/window_driver.hpp>
#include <Furrovine++/Input/mouse_device.hpp>
#include <Furrovine++/Input/keyboard_device.hpp>
#include <Furrovine++/Input/input_events.hpp>
#include <Furrovine++/Graphics/window.hpp>
#include <Furrovine++/Graphics/graphics_device.hpp>
#include <Furrovine++/Graphics/sprite_batch.hpp>
#include <Furrovine++/Graphics/image_2d.hpp>
#include <Furrovine++/Pipeline/TextureFontLoader.hpp>
#include <Furrovine++/Sys/file_watcher.hpp>
#include <Furrovine++/stopwatch.hpp>
#include <Furrovine++/intersect2.hpp>

Furrovine::string make_coords_info( Furrovine::Graphics::image_2d& image, const vec2i& coords ) {
	using namespace Furrovine;
	TVector2<uint32> ucoords( coords );
	if ( !intersect( image.boundaries( ), ucoords ) ) {
		return "Mouse over pixel...";
	}
	const Furrovine::ByteColor& color = image.view<ByteColor>()[ coords ];
	string coordsstring = Format( "Pixel - r: {0} | g: {1} | b: {2}",
		lexical_cast( static_cast<int>( color.r ) ),
		lexical_cast( static_cast<int>( color.g ) ),
		lexical_cast( static_cast<int>( color.b ) ) );
	return coordsstring;
}

Furrovine::string make_info( Furrovine::stopwatch<>& stopwatch, Furrovine::Graphics::image_2d& image, const vec2i& mouse, RayTracerStep steps ) {
	using namespace Furrovine;
	string stepstring = "Finished\n";
	if ( has_flags( steps, RayTracerStep::Preliminary ) ) {
		stepstring = "Running\nTracing Rays...";
	}
	else {
		if ( has_flags( steps, RayTracerStep::MultisampleDetection | RayTracerStep::Multisampling ) )
			stepstring = "Running\nDetecting and Tracing Multisample Rays...";
		else if ( has_flags( steps, RayTracerStep::MultisampleDetection ) )
			stepstring = "Running\nDetecting Multisample Rays...";
		else if ( has_flags( steps, RayTracerStep::Multisampling ) )
			stepstring = "Running\nTracing Multisampling Rays...";
	}

	long long elapsedmilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.elapsed()).count();
	int hours = static_cast<int>( elapsedmilliseconds / 1000 / 60 / 60 );
	int minutes = static_cast<int>( ( elapsedmilliseconds / 1000 / 60 ) - ( hours * 60 ) );
	int secs = static_cast<int>( ( elapsedmilliseconds / 1000 ) - ( hours * 60 * 60 ) - ( minutes * 60 ) );
	int milliseconds = static_cast<int>( (elapsedmilliseconds)-( hours * 60 * 60 * 1000 ) - ( minutes * 60 * 1000 ) - ( secs * 1000 ) );
	string hstring = lexical_cast( hours );
	string mstring = lexical_cast( minutes );
	string sstring = lexical_cast( secs );
	string msstring = lexical_cast( milliseconds );
	if ( hours < 10 )
		hstring.prepend( "0" );
	if ( minutes < 10 )
		mstring.prepend( "0" );
	if ( secs < 10 )
		sstring.prepend( "0" );
	if ( milliseconds < 10 )
		msstring.prepend( "00" );
	else if ( milliseconds < 100 )
		msstring.prepend( "0" );

	string timestring = Format( "{0}:{1}:{2}.{3}",
		hstring, mstring, sstring, msstring );
	string datastring = Format( "{0}\nMouse - x: {1} | y: {2}\n{3}\nTime - {4}",
		stepstring,
		lexical_cast( mouse.x ), lexical_cast( mouse.y ),
		make_coords_info( image, mouse ),
		timestring );
	return datastring;
}

template <typename TTracer>
void RayTrace( RayTracerCommand& command, Furrovine::stopwatch<>& stopwatch, Furrovine::optional<const Furrovine::string&> source, Furrovine::Graphics::image_2d& image, ImageOutput& output, TTracer&& raytracer ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	using namespace Furrovine::Sys;
	using namespace Furrovine::enums::operators;
	uint32 bottombar = 100;
	vec2 imagesize( static_cast<real>( command.imagesize.x ), static_cast<real>( command.imagesize.y ) );
	Size2u32 windowsize( command.imagesize.x, command.imagesize.y + bottombar );
	real offsetx = 220.0f;
	
	vec2u magtoolsize( 32, 32 );
	vec2u magsize( 16, 16 );
	vec2 magpixelsize( 4.0f, 4.0f );
	vec2 magoffset( offsetx + 60.0f, imagesize.y + ( static_cast<real>(bottombar)-( magsize.y * magpixelsize.y ) ) / 2.0f );

	window_driver windowdriver( window_driver_flags::AlwaysReceiveInput | window_driver_flags::ManipulateInputDevices  );
	window displaywindow( windowdriver, window_description( "Lightstalker", windowsize ) );
	graphics_device graphics( displaywindow );
	input_events<unit> inputevents;
	text_device text( displaywindow );
	sprite_batch batch( graphics );
	queue<message> messagequeue;
	texture_font font = TextureFontLoader( graphics, text )( texture_font_description( "C:/Windows/Fonts/Arial.ttf", 10 ) );
	keyboard_device keyboard( 0 );
	mouse_device mouse( 0 );
	vec2i mousepos( 0, 0 );

	file_watcher watcher( maybe_or( source, string( "" ) ), "", false );
	
	std::vector<byte> watcherbuffer( 2048 * 4 );
	std::vector<file_delta> changes;
	changes.reserve( 4 );

	bool quit = false;
	bool autosaved = false;
	bool doreset = false;
	bool doreload = false;
	bool& shoulddisplaywindow = command.displaywindow;
	if ( shoulddisplaywindow ) {
		displaywindow.Show( );
	}

	while ( true ) {
		if ( quit ) {
			windowdriver.quit( displaywindow );
		}
		windowdriver.push( displaywindow, messagequeue );
		optional<message> opmessage;
		while ( opmessage = messagequeue.pop_front( ) ) {
			message& message = opmessage.get( );
			inputevents.process( message );
			switch ( message.class_index() ) {
			case message::index<window_event>::value: {
				window_event& windowm = message.get<window_event>( );
				quit = windowm.signal == window_event_signal::quit
					|| windowm.signal == window_event_signal::destroy;
				break; }
			}
		}

		if ( quit )
			break;
		
		mouse.update( );
		keyboard.update( );
		if ( keyboard.pressed( Key::Escape ) ) {
			quit = true;
		}
		if ( keyboard.control_down() ) {
			if ( keyboard.pressed( Key::Q ) ) {
				quit = true;
			}
			if ( keyboard.pressed( Key::R ) ) {
				doreset = true;
				doreload = static_cast<bool>( source );
			}
			if ( keyboard.pressed( Key::S ) ) {
				output.save( );
			}
		}
		mousepos = mouse.position( );

		if ( source ) {
			watcher.wait( changes, watcherbuffer, Timeout( 33 ) );
			if ( changes.size( ) > 0 ) {
				for ( auto& change : changes ) {
					if ( change.timed_out )
						continue;
					if ( any_flags( change.changes, file_change_flags::modified | file_change_flags::created )
						|| ( any_flags( change.changes, file_change_flags::renamed ) && change.name == *source ) ) {
						doreset = true;
						doreload = true;
						break;
					}
				}
				changes.clear( );
			}
		}

		if ( doreset ) {
			stopwatch.stop( );
			raytracer.Stop( );
			output.Clear( );
			if ( doreload ) {
				vec2u reloadedimagesize = command.imagesize;
				command = std::move( RayTracerCommandLoader( )( *source ) );
				command.imagesize = reloadedimagesize;
				command.scene.Build( );
				doreload = false;
			}
			stopwatch.start( );
			raytracer.Reset( );
			doreset = false;
			continue;
		}

		raytracer.Compute( );

		if ( raytracer.Check( ) ) {
			stopwatch.stop( );
		}

		if ( !shoulddisplaywindow || !graphics.Ready( ) ) {
			continue;
		}

		auto imageview = image.view<ByteColor>( );
		texture_2d imagetexture( graphics, image );

		graphics.clear( Color::Black );
		batch.begin();
		batch.render( imagetexture, none, Region( 0.0f, 0.0f, imagesize.x, imagesize.y ), Color::White );
		string datastring = make_info( stopwatch, image, mousepos, raytracer.Steps() );
		batch.render_text( font, datastring, { 0, imagesize.y } );
		auto backcolor = rgba::AmbientGrey;
		
		if ( mouse.down( mouse_button::Right ) ) {
			vec2 backpos( mousepos );
			backpos -= magpixelsize * static_cast<real>( 12 );
			backpos -= static_cast<real>( 1 );
			batch.render_gradient( Region( backpos, size2( vec2( magtoolsize ) * magpixelsize + static_cast<real>( 2 ) ) ), backcolor );
			for ( std::size_t y = 0; y < magtoolsize.x; ++y ) {
				for ( std::size_t x = 0; x < magtoolsize.y; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					vec2u pos( mousepos.x + x, mousepos.y + y );
					pos -= magtoolsize / static_cast<std::size_t>( 2 );
					auto color = !intersect( image.boundaries( ), pos ) ? rgba::Black : imageview[ pos ];
					vec2 renderpos( static_cast<real>( mousepos.x ), static_cast<real>( mousepos.y ) );
					renderpos -= magpixelsize * static_cast<real>( 12 );
					renderpos += magpixelsize * vec2( sx, sy );
					Region region( renderpos, size2( magpixelsize ) );
					batch.render_gradient( region, color );
				}
			}
		}
		else {
			batch.render_gradient( Region( magoffset - vec2( 1.0f, 1.0f ), size2( vec2( magsize ) * magpixelsize + vec2( 2.0f, 2.0f ) ) ), backcolor );
			for ( std::size_t y = 0; y < magsize.y; ++y ) {
				for ( std::size_t x = 0; x < magsize.x; ++x ) {
					real sx = static_cast<real>( x );
					real sy = static_cast<real>( y );
					vec2u pos( mousepos.x + x, mousepos.y + y );
					pos -= magsize / static_cast<std::size_t>( 2 );
					auto color = !intersect( image.boundaries( ), pos ) ? rgba::Black : imageview[ pos ];
					color.a = 1.0f;
					vec2 renderpos( magoffset );
					renderpos += magpixelsize * vec2( sx, sy );
					Region region( renderpos, size2( magpixelsize ) );
					batch.render_gradient( region, color );
				}
			}
		}

		batch.end( );
		graphics.present( );

		if ( raytracer.Check( ) ) {
			if ( !autosaved ) {
				autosaved = true;
				// TODO: triggers a crash in release. Find out why.
				output.save( );
				if ( !shoulddisplaywindow ) {
					quit = true;
					continue;
				}
			}
		}
	}
}

#include <iostream>

int main( int argc, char* argv[] ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Threading;
	
	std::vector<string_view> arguments( argv, argv + argc );
	
	optional<string> source = nullopt;
	optional<string> output = nullopt;
	optional<RayTracerCommand> ocommand = nullopt;
	if ( arguments.size( ) > 1 ) {
		source = arguments[ 1 ];
		if ( arguments.size( ) > 2 ) {
			output = arguments[ 2 ];
		}
	}
#ifndef _DEBUG
	try {
#endif // Debug
	if ( !output ) {
		if ( source ) {
			output = *source + ".png";
		}
		else {
			output = "output.png";
		}
	}
	if ( source )
		ocommand = RayTracerCommandLoader()( *source );
	if (!ocommand)
		ocommand = SampleCommands::MultiSphere( );
	stopwatch<> stopwatch;
	RayTracerCommand& command = *ocommand;
	image_2d image( command.imagesize, surface_format::Red8Green8Blue8Alpha8Normalized, to_byte_size( surface_format::Red8Green8Blue8Alpha8Normalized ), 0 );
	ImageOutput imageoutput( image, *output );
	Scene& scene = command.scene;
	Camera& camera = command.camera;
	RayBouncer& bouncer = command.bouncer;
	RayShader& shader = command.shader;
	optional<Multisampler>& multisampler = command.multisampler;
	vec2u& imagesize = command.imagesize;
	scene.Build( );
	// TODO: find out why this next line crashes VC++'s compiler
	if ( command.multithreading ) {
		ThreadPool threadpool( command.threadcount );
		ThreadedTileTracer<16, 16> raytracer(threadpool, imagesize, camera,
			scene, bouncer, shader, multisampler, imageoutput);
		stopwatch.start( );
		RayTrace( command, stopwatch, source, image, imageoutput, raytracer );
	}
	else {
		TileTracer<16, 16> raytracer( imagesize, camera,
			scene, bouncer, shader, multisampler, imageoutput, std::chrono::milliseconds( 1500 ) );
		stopwatch.start( );
		RayTrace( command, stopwatch, source, image, imageoutput, raytracer );
	}
#ifndef _DEBUG
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
	std::cout << "Lightstalker - Completed. Exiting." << std::endl;
#endif // Debug
	return 0;
}
