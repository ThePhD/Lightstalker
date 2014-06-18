#include "RayTracerCommandLoader.h"
#include "ObjLoader.h"
#include "FilmSize.h"
#include <Furrovine++/IO/FileStream.h>
#include <Furrovine++/IO/TextReader.h>

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
	AmbientLight alight;
	DirectionalLight dlight;
	PointLight plight;
	vec3 cpos( real_zero, real_zero, real(-100 ) );
	vec3 cdir( 0, 0, -1.0f );
	vec3 cup = vec3::Up;
	vec2 framesize = FilmSize::FullFrame;
	float cfocal = 420.0f;
	Fur::String stringvalue = "";
	Fur::uint32 uintvalue = 0;
	bool cameracommanded = false;

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
			if ( reader.ReadSingle( triangle.a.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.a.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.b.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.b.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.b.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.c.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.c.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( reader.ReadSingle( triangle.c.z ) )
				break;
			scene.AddPrimitive( triangle );
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
			reader.ReadToNewLine( );
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
		case 'o':
			if ( reader.At( "bj", true ) ) {
				reader.SkipBlankSpace( );
				reader.ReadToNewLine( stringvalue );
				stringvalue.Trim( );
				objloader( stringvalue );
				break;
			}
			// Special options
			while ( !reader.AtNewLine( ) ) {
				reader.SkipBlankSpace( );
				if ( reader.At( "ms", true ) ) {
					reader.SkipBlankSpace( );
					if ( reader.ReadUInt32( uintvalue ) ) {
						Fur::uint32 xsamples = uintvalue;
						Fur::uint32 ysamples = uintvalue;
						if ( reader.ReadUInt32( uintvalue ) ) {
							ysamples = uintvalue;
						}
						std::default_random_engine randomengine{ };
						command.multisampler = Fur::optional<Multisampler>( Fur::in_place, xsamples, ysamples, randomengine );
					}
				}
				else if ( reader.At( "shadows", true ) ) {
					command.shader.Shadows = true;
				}
				else if ( reader.At( "no-shadows", true ) ) {
					command.shader.Shadows = false;
				}
				else if ( reader.At( "multhreading", true ) ) {
					command.multithreading = true;
				}
				else if ( reader.At( "no-multhreading", true ) ) {
					command.multithreading = false;
				}
				else {
					reader.Read( );
				}
			}
			break;
		case 'c':
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cpos.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cpos.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cpos.z ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdir.x ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdir.y ) )
				break;
			reader.SkipBlankSpace( );
			if ( !reader.ReadSingle( cdir.z ) )
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
			
			camera = Camera( cpos, cdir, cup, cfocal, framesize );
			cameracommanded = true;
			break;
		default:
			reader.SkipLine( );
			break;
		}
		
		reader.Read( );
		if ( reader.EoF( ) ) {
			break;
		}
	}

	if ( !cameracommanded ) {
		// Generate a default camera based on the scene
		::BoundingBox box = scene.Bounds( );
		cpos = box.max * static_cast<real>( 3 );
		cdir = cpos.direction_to( box.Center( ) );
		if ( cdir == cup || cdir == -cup )
			cup = vec3::Forward;
		camera = Camera( cpos, cdir, cup, cfocal, framesize );
	}

	return command;
}