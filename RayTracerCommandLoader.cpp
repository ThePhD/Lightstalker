#include "RayTracerCommandLoader.hpp"
#include "ObjLoader.hpp"
#include "FilmSize.hpp"
#include <Furrovine++/IO/file_stream.hpp>
#include <Furrovine++/IO/text_reader.hpp>
#include <Furrovine++/IO/file.hpp>
#include <Furrovine++/Graphics/primitives.hpp>
#include <Furrovine++/text_algorithm.hpp>

RayTracerCommand RayTracerCommandLoader::operator()( const Fur::string& file ) {
	using namespace Furrovine::IO;
	file_stream stream( file, file_mode::Open );
	return ( *this )( stream );
}

RayTracerCommand RayTracerCommandLoader::operator()( Fur::IO::stream& stream ) {
	using namespace Furrovine::IO;
	text_reader<> reader( stream );
	return ( *this )( reader );
}

RayTracerCommand RayTracerCommandLoader::operator()( Fur::IO::text_reader<>& reader ) {
	using namespace Furrovine::Graphics;
	RayTracerCommand command{ };
	Triangle triangle;
	Plane plane;
	Sphere sphere;
	Box box;
	AmbientLight alight;
	DirectionalLight dlight;
	PointLight plight;
	BasicMaterial material;
	rgba background( rgba::White );
	vec3 cpos( 0.0f, 0.0f, -100.0f );
	vec3 cdirtarget( 0.0f, 0.0f, -1.0f );
	vec3 cup = vec3::Up;
	vec2 framesize = FilmSize::FullFrame;
	float cfocal = 48.0f;
	Fur::string stringvalue = "";
	Fur::uint32 uintvalue = 0;
	bool cameracommanded = false;
	bool lookat = false;
	Furrovine::Graphics::winding_order order = winding_order::Collinear;

	Scene& scene = command.scene;
	RayShader& shader = command.shader;
	Camera& camera = command.camera;
	ObjLoader objloader( scene );

	while ( true ) {
		Fur::optional<Fur::code_point> maybep = reader.Peek( );
		if ( !maybep )
			break;
		reader.Read( );
		Fur::code_point c = maybep.get();
		switch ( c ) {
		case '/':
			reader.SkipLine( );
			break;
		case 's':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( sphere.origin.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( sphere.origin.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( sphere.origin.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( sphere.radius ) )
				break;
			scene.AddPrimitive( sphere );
			break;
		case 't':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.a.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.a.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.a.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.b.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.b.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.b.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.c.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.c.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( triangle.c.z ) )
				break;
			order = Fur::Graphics::primitives::triangle_winding( triangle.a, triangle.b, triangle.c, triangle.normal( ) );
			scene.AddPrimitive( triangle );
			break;
		case 'b':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.min.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.min.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.min.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.max.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.max.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( box.max.z ) )
				break;
			for ( std::size_t i = 0; i < box.max.size( ); ++i ) {
				if ( box.min[ i ] > box.max[ i ] )
					std::swap( box.min[ i ], box.max[ i ] );
			}
			scene.AddPrimitive( box );
			break;
		case 'p':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( plane.normal.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( plane.normal.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( plane.normal.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( plane.distance ) )
				break;
			
			scene.AddPrimitive( plane );
			reader.SkipLine( );
			break;
		case 'l':
			reader.SkipBlankSpace( );
			maybep = reader.Peek( );
			if ( !maybep )
				break;
			reader.Read( );
			c = maybep.get();
			switch ( c ) {
			case 'a':
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( alight.intensity.r ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( alight.intensity.g ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( alight.intensity.b ) )
					break;
				alight.intensity.a = 1.0f;
				scene.AddAmbientLight( alight );
				break;
			case 'p':
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.position.x ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.position.y ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.position.z ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.intensity.r ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.intensity.g ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( plight.intensity.b ) )
					break;
				plight.intensity.a = 1.0f;
				scene.AddPointLight( plight );
				break;
			case 'd':
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.direction.x ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.direction.y ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.direction.z ) )
					break;
				dlight.direction.normalize( );
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.intensity.r ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.intensity.g ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( dlight.intensity.b ) )
					break;
				dlight.intensity.a = 1.0f;
				scene.AddDirectionalLight( dlight );
				break;
			}
			reader.ReadToNewLine( );
			break;
		case 'm':
			material.matrefractivity = rgba::White;
			material.matreflectivity = rgba::Transparent;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matcolor.r ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matcolor.g ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matcolor.b ) )
				break;

			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matspecular.r ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matspecular.g ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matspecular.b ) )
				break;

			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( material.matspecularpower ) )
				break;

			do {
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matreflectivity.r ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matreflectivity.g ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matreflectivity.b ) )
					break;

				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matrefractivity.r ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matrefractivity.g ) )
					break;
				reader.SkipBlankSpace( );
				if ( !reader.ReadSingle( material.matrefractivity.b ) )
					break;
			} while ( false );

			scene.AddMaterial( material );
			break;
		case 'c':
			reader.SkipBlankSpace( );
			maybep = reader.Peek( );
			if ( !maybep )
				break;
			c = maybep.get();
			if ( c == 't' ) {
				reader.Read( );
				reader.ReadBlankSpace( );
				lookat = true;
			}
			if ( !reader.ReadSingle( cpos.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cpos.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cpos.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdirtarget.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdirtarget.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdirtarget.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cfocal ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( framesize.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( framesize.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadUInt32( uintvalue ) )
				break;
			command.imagesize.x = uintvalue;
			reader.SkipBlankSpace( );
			if ( !reader.ReadUInt32( uintvalue ) )
				break;
			command.imagesize.y = uintvalue;
			
			if ( lookat ) {
				if ( cdirtarget == cup || cdirtarget == -cup )
					cup = vec3::Forward;
				camera = Camera( camera_look_at, cpos, cdirtarget, cup, cfocal, framesize );
			}
			else {
				if ( cdirtarget == cup || cdirtarget == -cup )
					cup = vec3::Forward;
				camera = Camera( cpos, cdirtarget, cup, cfocal, framesize );
			}
			cameracommanded = true;
			break;
		case 'v':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( background.r ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( background.g ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( background.b ) )
				break;
			scene.SetBackground( background );
			break;
		case 'o':
			if ( reader.At( "bj", true ) ) {
				reader.SkipBlankSpace( );
				reader.ReadToNewLine( stringvalue );
				Fur::trim( stringvalue );
				if ( Fur::IO::file::Exists( stringvalue ) )
					objloader( stringvalue );
				break;
			}
			// Special options
			while ( !reader.AtNewLine( ) ) {
				reader.SkipBlankSpace( );
				if ( reader.At( "multisamples", true ) ) {
					reader.SkipBlankSpace( );
					if ( reader.ReadUInt32( uintvalue ) ) {
						Fur::uint32 xsamples = uintvalue;
						Fur::uint32 ysamples = uintvalue;
						reader.SkipBlankSpace( );
						if ( reader.ReadUInt32( uintvalue ) ) {
							ysamples = uintvalue;
						}
						std::default_random_engine randomengine{ };
						command.multisampler = Multisampler( xsamples, ysamples, randomengine );
					}
				}
				else if ( reader.At( "display", true ) ) {
					command.displaywindow = true;
				}
				else if ( reader.At( "no-display", true ) ) {
					command.displaywindow = false;
				}
				else if ( reader.At( "shadows", true ) ) {
					command.shader.Shadows = true;
				}
				else if ( reader.At( "no-shadows", true ) ) {
					command.shader.Shadows = false;
				}
				else if ( reader.At( "multithreading", true ) ) {
					command.multithreading = true;
					reader.SkipBlankSpace( );
					if ( reader.ReadUInt32( uintvalue ) ) {
						uintvalue = std::max<Fur::uint32>( 2, uintvalue );
						command.threadcount = uintvalue;
					}
				}
				else if ( reader.At( "no-multithreading", true ) ) {
					command.multithreading = false;
				}
				else {
					reader.Read( );
				}
			}
			break;
		default:
			break;
		}
		
		if ( reader.eos( ) ) {
			break;
		}
	}

	if ( !cameracommanded ) {
		// Generate a default camera based on the scene
		Box defaultbox = scene.Bounds( );
		cpos = defaultbox.max * static_cast<real>( 3 );
		cdirtarget = cpos.direction_to( box.center( ) );
		if ( cdirtarget == cup || cdirtarget == -cup )
			cup = vec3::Forward;
		camera = Camera( cpos, cdirtarget, cup );
	}

	return command;
}