#pragma once

#include "Scene.hpp"
#include "Material.hpp"
#include "ObjLoader.hpp"
#include "RayTracerCommandLoader.hpp"

namespace SampleCommands {

	RayTracerCommand Trifecta( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::WhiteSmoke );
		scene.Add( BasicMaterial( rgba::Red, rgba::White, rgba::White, 32.0f,
			rgba( 0.7f, 0.7f, 0.7f, 0.7f ), rgba::Transparent, rgba::Transparent, Ior::Water ),
			sphere_arg, 80.0f, vec3( 0.0f, 120.0f, 138.564f - 69.282f ) );
		scene.Add( BasicMaterial( rgba::Blue, rgba::White, rgba::White, 32.0f ),
		sphere_arg, 80.0f, vec3( 80.0f, 120.0f, 0.0f - 69.282f ) );
		scene.Add( BasicMaterial( rgba::Yellow, rgba::White, rgba::White, 32.0f ),
		sphere_arg, 80.0f, vec3( -80.0f, 120.0f, 0.0f - 69.282f ) );
		
		scene.Add( BasicMaterial( rgba::BlueBell, rgba::White, rgba::Transparent ),
		plane_arg, -30.0f, vec3::Up );
		
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );

		camera = Camera( camera_look_at, vec3( 0.0f, 40.0f, -500.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up );

		return command;
	}

	RayTracerCommand MultiSphere( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::WhiteSmoke );
		scene.Add( BasicMaterial{ rgba::Red, rgba::White, rgba::White, 32.0f,
			rgba( 0.5f, 0.5f, 0.5f, 0.5f ) },
			sphere_arg, 80.0f, vec3( 0.0f, 120.0f, 138.564f - 69.282f ) );
		scene.Add( BasicMaterial{ rgba::Blue, rgba::White, rgba::White, 32.0f },
		sphere_arg, 80.0f, vec3( 80.0f, 120.0f, 0 - 69.282f ) );
		scene.Add( BasicMaterial{ rgba::Yellow, rgba::White, rgba::White, 32.0f },
		sphere_arg, 80.0f, vec3( -80.0f, 120.0f, 0 - 69.282f ) );
		
		for ( std::ptrdiff_t x = -2; x < 3; ++x ) {
			for ( std::ptrdiff_t z = -2; z < 3; ++z ) {
				real xspace = 80;
				real zspace = 80;
				scene.Add( BasicMaterial{ rgba::Blue, rgba::White, rgba::White, 32.0f },
					sphere_arg, 20.0f,
					vec3( ( xspace * x ), -10.0f, 138.564f - 69.282f + ( zspace * z ) ) );
			}
		}

		scene.Add( BasicMaterial{ rgba::BlueBell, rgba::White, rgba::Transparent },
			plane_arg, -30.0f, vec3::Up );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );

		camera = Camera( camera_look_at, vec3( 0.0f, 250.0f, -500.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up );
		
		return command;
	}

	RayTracerCommand RefractionTest( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::WhiteSmoke );
		scene.Add( BasicMaterial{ rgba::Red, rgba::White, rgba::White, 32,
			rgba( 0.5f, 0.5f, 0.5f, 0.5f ) },
			sphere_arg, 80.0f, vec3( 0.0f, 120.0f, 138.564f - 69.282f ) );

		scene.Add( BasicMaterial{ rgba::Blue, rgba::White, rgba::White, 32,
			rgba::White, rgba( 0.2f, 0.2f, 0.2f, 0.2f ) },
			sphere_arg, 60.0f, vec3( 0.0f, 120.0f, 138.564f - 69.282f ) );

		scene.Add( CheckerMaterial( rgba::Black, rgba::White, rgba::White, rgba::Transparent, 32.0f, rgba::White, rgba::Transparent,
			rgba::Transparent, Ior::Water, Absorption::Water, rgba::White, vec2( 80.0f, 80.0f ) ),
			plane_arg, -30.0f, vec3::Up );

		scene.Add( CheckerMaterial( rgba::LightBlue, rgba::White, rgba::White, rgba::Transparent, 32.0f, rgba::White, rgba::Transparent,
			rgba::Transparent, Ior::Water, Absorption::Water, rgba::White, vec2( 70.0f, 70.0f ) ),
			plane_arg, -200.0f, vec3::Right );

		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );

		camera = Camera( camera_look_at, vec3( 0.0f, 40.0f, -500.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up );
		
		return command;
	}

	RayTracerCommand SizedSpheres( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::WhiteSmoke );
		scene.Add( BasicMaterial{ rgba::Red, rgba::White, rgba::White, 32.0f, rgba::White, rgba(0.5f, 0.5f, 0.5f, 0.5f) },
			sphere_arg, 50.0f, vec3( 0.0f, 60.0f, 0.0f ) );
		scene.Add( BasicMaterial{ rgba::YellowOrange, rgba::White, rgba::White, 32.0f },
			sphere_arg, 80.0f, vec3( 0.0f, 90.0f, 130.0f ) );

		scene.Add( BasicMaterial{ rgba::LightBlue, rgba::White, rgba::White, 32.0f },
			sphere_arg, 30.0f, vec3( -90.0f, 40.0f, -10.0f ) );
		scene.Add( BasicMaterial{ rgba::DarkGreen, rgba::White, rgba::White, 32.0f },
			sphere_arg, 30.0f, vec3( 90.0f, 40.0f, -10.0f ) );

		scene.Add( BasicMaterial{ rgba::BlueBell, rgba::White, rgba::Transparent },
			plane_arg, -1.0f, vec3::Up );

		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );

		camera = Camera( camera_look_at, vec3( 0.0f, 40.0f, -500.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up );
		
		return command;
	}

	RayTracerCommand Complex( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::WhiteSmoke );
		
		// ground plane
		scene.Add( BasicMaterial{ rgba( 0.4f, 0.3f, 0.3f, 1.0f ), rgba::White, rgba::White, 32 },
			plane_arg, 4.4f, vec3::Up );
		
		// big sphere
		scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f, 1.0f ), rgba::White, rgba::White, 32,
				rgba( 0.2f, 0.2f, 0.2f, 0.2f ), rgba( 0.8f, 0.8f, 0.8f, 0.8f ), rgba::Transparent, 1.3f },
			sphere_arg, 2.5f, vec3( 2.0f, 0.8f, 3.0f ) );
		
		// small sphere
		scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f ),
				rgba( 0.1f, 0.1f, 0.1f, 0.1f ),
				rgba::White,
				32.0f,
				rgba( 0.0f, 0.0f, 0.0f, 0.0f ),
				rgba( 0.5f, 0.5f, 0.5f, 0.5f ), },
			sphere_arg, 2.0f, vec3( -5.5f, -0.5f, 7.0f ) );
		
		// light source 1
		scene.AddPointLight( vec3( 0.0f, 5.0f, 5.0f ), rgba( 0.4f, 0.4f, 0.4f, 0.4f ) );
		// light source 2
		scene.AddPointLight( vec3( -3.0f, 5.0f, 1.0f ), rgba( 0.6f, 0.6f, 0.8f, 0.8f ) );
		
		// grid
		int prim = 8;
		for ( int x = 0; x < 8; x++ ) {
			for ( int y = 0; y < 7; y++ ) {
				scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					32,
					rgba( 0.0f, 0.0f, 0.0f, 0.0f ),
					rgba( 0.0f, 0.0f, 0.0f, 0.0f ) },
					sphere_arg, 0.3f, vec3( -4.5f + x * 1.5f, -4.3f + y * 1.5f, 10.0f ) );
			}
		}

		camera = Camera( vec3( 0.0f, 10.0f, -10.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up, 500.0f );

		return command;
	}

	RayTracerCommand ObjScene( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::SlateGrey );
		ObjLoader obj( scene );
		obj( "Mesh/CubeSphere.obj" );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );
		
		camera = Camera( vec3( 3.0f, 3.0f, 3.0f ) * scene.Bounds( ).max, vec3( 0.0f, 0.0f, 0.0f ), vec3::Up, 500.0f );
		
		return command;
	}

	RayTracerCommand SimpleObjScene( ) {
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( rgba::SlateGrey );
		ObjLoader obj( scene );
		obj( "Mesh/Triangle.obj" );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0.0f, -1.0f, 0.0f ) ), rgba::White ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1.0f, -1.0f, 0.0f ) ), rgba::White ) );
		
		camera = Camera( vec3( 0.0f, 0.0f, -500.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3::Up, 500.0f );
		
		return command;
	}

}