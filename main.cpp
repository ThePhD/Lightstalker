#if 0
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
#include <Furrovine++/Input/KeyboardDevice.hpp>
#include <Furrovine++/Input/input_events.hpp>
#include <Furrovine++/Graphics/window.hpp>
#include <Furrovine++/Graphics/graphics_device.hpp>
#include <Furrovine++/Graphics/NymphBatch.hpp>
#include <Furrovine++/Graphics/image_2d.hpp>
#include <Furrovine++/Pipeline/TextureFontLoader.hpp>
#include <Furrovine++/Sys/FileWatcher.hpp>
#include <Furrovine++/stopwatch.hpp>
#include <Furrovine++/intersect2.hpp>

Furrovine::string make_coords_info( Furrovine::Graphics::Image2D& image, const vec2i& coords ) {
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

Furrovine::string make_info( Furrovine::stopwatch<>& stopwatch, Furrovine::Graphics::Image2D& image, const vec2i& mouse, RayTracerStep steps ) {
	using namespace Furrovine;
	string stepstring = "Finished\n";
	if ( Fur::has_flags( steps, RayTracerStep::Preliminary ) ) {
		stepstring = "Running\nTracing Rays...";
	}
	else {
		if ( Fur::has_flags( steps, RayTracerStep::MultisampleDetection | RayTracerStep::Multisampling ) )
			stepstring = "Running\nDetecting and Tracing Multisample Rays...";
		else if ( Fur::has_flags( steps, RayTracerStep::MultisampleDetection ) )
			stepstring = "Running\nDetecting Multisample Rays...";
		else if ( Fur::has_flags( steps, RayTracerStep::Multisampling ) )
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
void RayTrace( RayTracerCommand& command, Furrovine::stopwatch<>& stopwatch, Furrovine::optional<const Furrovine::string&> source, Furrovine::Graphics::Image2D& image, ImageOutput& output, TTracer&& raytracer ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	using namespace Furrovine::Text;
	using namespace Furrovine::Input;
	using namespace Furrovine::Sys;
	using namespace Furrovine::enums::operators;
	uint32 bottombar = 100;
	vec2 imagesize( static_cast<real>( command.imagesize.x ), static_cast<real>( command.imagesize.y ) );
	Fur::Size2u32 windowsize( command.imagesize.x, command.imagesize.y + bottombar );
	real offsetx = 220.0f;
	
	vec2u magtoolsize( 32, 32 );
	vec2u magsize( 16, 16 );
	vec2 magpixelsize( 4.0f, 4.0f );
	vec2 magoffset( offsetx + 60.0f, imagesize.y + ( static_cast<real>(bottombar)-( magsize.y * magpixelsize.y ) ) / 2.0f );

	WindowDriver windowdriver( Fur::WindowDriverFlags::Default );
	Window window( windowdriver, Fur::WindowDescription( "Lightstalker", windowsize ) );
	GraphicsDevice graphics( window );
	InputEvents<unit> inputevents;
	TextDevice text( window );
	NymphBatch batch( graphics );
	queue<Message> messagequeue;
	TextureFont font = TextureFontLoader( graphics, text )( TextureFontDescription( "C:/Windows/Fonts/Arial.ttf", 10 ) );
	KeyboardDevice keyboard( 0 );
	MouseDevice mouse( 0 );
	vec2i mousepos( 0, 0 );

	FileWatcher watcher( maybe_or( source, string( "" ) ), "", false );
	
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
		Fur::optional<Fur::Message> opmessage;
		while ( opmessage = messagequeue.pop_front( ) ) {
			Fur::Message& message = opmessage.get( );
			inputevents.Process( message );
			switch ( message.class_index() ) {
			case Fur::Message::index<Fur::WindowEvent>::value: {
				Fur::WindowEvent& windowm = message.get<Fur::WindowEvent>( );
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
					if ( any_flags( change.Changes, FileChangeFlags::Modified | FileChangeFlags::Created )
						|| ( any_flags( change.Changes, FileChangeFlags::Renamed ) && change.Name == *source ) ) {
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

		if ( !displaywindow || !graphics.Ready( ) ) {
			continue;
		}

		auto imageview = image.view<ByteColor>( );
		
		graphics.Clear( Color::Black );
		graphics.RenderImage( image, Region( 0.0f, 0.0f, imagesize.x, imagesize.y ) );
		batch.Begin( );
		string datastring = make_info( stopwatch, image, mousepos, raytracer.Steps() );
		batch.RenderString( font, datastring, { 0, imagesize.y } );
		auto backcolor = rgba::AmbientGrey;
		
		if ( mouse.Down( MouseButton::Right ) ) {
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
					auto color = !intersect( image.boundaries( ), pos ) ? rgba::Black : imageview[ pos ];
					vec2 renderpos( static_cast<real>( mousepos.x ), static_cast<real>( mousepos.y ) );
					renderpos -= magpixelsize * static_cast<real>( 12 );
					renderpos += magpixelsize * vec2( sx, sy );
					Region region( renderpos, size2( magpixelsize ) );
					batch.RenderGradient( region, color );
				}
			}
		}
		else {
			batch.RenderGradient( Region( magoffset - vec2( 1.0f, 1.0f ), size2( vec2( magsize ) * magpixelsize + vec2( 2.0f, 2.0f ) ) ), backcolor );
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
					batch.RenderGradient( region, color );
				}
			}
		}

		batch.End( );
		graphics.Present( );

		if ( raytracer.Check( ) ) {
			if ( !autosaved ) {
				autosaved = true;
				// TODO: triggers a crash in release. Find out why.
				output.Save( );
				if ( !displaywindow ) {
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
		Image2D image( command.imagesize, SurfaceFormat::Red8Green8Blue8Alpha8Normalized, ToByteSize( SurfaceFormat::Red8Green8Blue8Alpha8Normalized ), 0 );
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
	
	return 0;
}
#else
#pragma warning(disable:4503)

#include <Furrovine++/Graphics/window.hpp>
#include <Furrovine++/Graphics/graphics_device.hpp>
#include <Furrovine++/Graphics/NymphBatch.hpp>
#include <Furrovine++/Input/input_events.hpp>
#include <Furrovine++/Pipeline/ImageLoader.hpp>
#include <Furrovine++/Pipeline/TextureFontLoader.hpp>
#include <Furrovine++/Graphics/image_2d.hpp>
#include <Furrovine++/queue.hpp>

int main( int argc, char * const argv[] ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Input;
	using namespace Furrovine::Pipeline;

	window_driver wd;
	window w( wd, window_description( "Furrovine", { 640, 480 } ) );
	input_events<unit> ipe;
	queue<message> messagequeue;

	graphics_device g( w );
	text_device t( w );

	NymphBatch batch( g );

	Color clears[] = {
		Color::Black,
		Color::Grey,
		Color::SlateGrey,
		Color::DarkGrey
	};

	Color colors[] = {
		Color::Red,
		Color::Blue,
		Color::Green,
		Color::PurpleCSS
	};

	image_2d image = ImageLoader()(load_single, "test.wbmp");
	texture_font font = TextureFontLoader( g, t )(texture_font_description( "Arial", 12.0f ));
	//image_2d image2 = ImageLoader()(load_single, "black.jpg");
	texture_2d tex( g, image );
	//texture_2d tex2( g, image2 );

	utf32_string x = "Arf";
	utf32_string_view xview = x;

	for ( bool quit = false; !quit; ) {

		wd.Push( w, messagequeue );
		optional<message> opmessage;
		while ( opmessage = messagequeue.pop_front() ) {
			message& msg = opmessage.get();
			ipe.Process( msg );
			switch ( msg.class_index() ) {
			case message::index<window_event>::value: {
				window_event& windowm = msg.get<window_event>();
				quit = windowm.Signal == window_event_signal::Quit
					|| windowm.Signal == window_event_signal::Destroy;
				break; }
			}
		}

		uintz target = rand() % size_of( clears );
		const Color& clearcolor = clears[ 0 ];
		const Color& triangle1color = colors[ 0 ];
		const Color& triangle2color = colors[ 1 ];

		g.clear( clearcolor );

		batch.Begin();
		batch.Render( tex, none, Region( 200, 200, 300, 300 ), Color::White );
		//batch.Render( tex2, none, Region( 260, 200, 50, 50 ), Color::White );
		batch.RenderGradient( Region( 50, 50, 100, 100 ), triangle1color, triangle2color );
		batch.RenderGradient( Region( 50, 100, 100, 100 ), triangle1color, triangle2color );
		batch.RenderString( font, "We are online!", Vector2( 100, 20 ) );
		batch.End();

		g.Present();
	}
}

#endif