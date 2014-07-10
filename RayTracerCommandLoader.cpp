#include "RayTracerCommandLoader.h"
#include "ObjLoader.h"
#include "FilmSize.h"
#include <Furrovine++/IO/FileStream.h>
#include <Furrovine++/IO/TextReader.h>
#include <Furrovine++/IO/File.h>
#include <Furrovine++/Graphics/Primitives.h>

RayTracerCommand RayTracerCommandLoader::operator()( const Fur::String& file ) {
	using namespace Furrovine::IO;
	FileStream stream( file, FileMode::Open );
	return ( *this )( stream );
}

RayTracerCommand RayTracerCommandLoader::operator()( Fur::IO::Stream& stream ) {
	using namespace Furrovine::IO;
	TextReader reader( stream );
	return ( *this )( reader );
}

RayTracerCommand RayTracerCommandLoader::operator()( Fur::IO::TextReader& reader ) {
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
	rgba background( RealWhite );
	vec3 cpos( real_zero, real_zero, real(-100 ) );
	vec3 cdirtarget( 0, 0, -1.0f );
	vec3 cup = vec3::Up;
	vec2 framesize = FilmSize::FullFrame;
	float cfocal = 48.0f;
	Fur::String stringvalue = "";
	Fur::uint32 uintvalue = 0;
	bool cameracommanded = false;
	bool lookat = false;
	Furrovine::Graphics::WindingOrder order = WindingOrder::Collinear;

	Scene& scene = command.scene;
	RayShader& shader = command.shader;
	Camera& camera = command.camera;
	ObjLoader objloader( scene );

	while ( true ) {
		int p = reader.Peek( );
		if ( p == -1 )
			break;
		reader.Read( );
		Fur::codepoint c = static_cast<Fur::codepoint>( p );
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
			order = Fur::Graphics::Primitives::TriangleWinding( triangle.a, triangle.b, triangle.c, triangle.normal( ) );
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
			p = reader.Peek( );
			if ( p == -1 )
				break;
			reader.Read( );
			c = static_cast<Fur::codepoint>( p );
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
			material.matrefractivity = RealWhite;
			material.matreflectivity = RealTransparent;
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
			p = reader.Peek( );
			if ( p == -1 )
				break;
			c = static_cast<Fur::codepoint>( p );
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
				stringvalue.Trim( );
				if ( Fur::IO::File::Exists( stringvalue ) )
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
		
		if ( reader.EoF( ) ) {
			break;
		}
	}

	if ( !cameracommanded ) {
		// Generate a default camera based on the scene
		Box box = scene.Bounds( );
		cpos = box.max * static_cast<real>( 3 );
		cdirtarget = cpos.direction_to( box.center( ) );
		if ( cdirtarget == cup || cdirtarget == -cup )
			cup = vec3::Forward;
		camera = Camera( cpos, cdirtarget, cup );
	}

	return command;
}