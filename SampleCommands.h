#pragma once

#include "Scene.h"
#include "Material.h"
#include "ObjLoader.h"
#include "RayTracerCommandLoader.h"
#include <Furrovine++/Colors.h>

namespace SampleCommands {

	RayTracerCommand Trifecta( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( WhiteSmoke );
		scene.Add( BasicMaterial( Red, White, White, 32,
			rgba( 0.7f, 0.7f, 0.7f, 0.7f ), Transparent, Transparent, Ior::Water ),
			sphere_arg, 80.0f, vec3( 0, 120, 138.564f - 69.282f ) );
		scene.Add( BasicMaterial( Blue, White, White, 32 ),
		sphere_arg, 80.0f, vec3( 80, 120, 0 - 69.282f ) );
		scene.Add( BasicMaterial( Yellow, White, White, 32 ),
		sphere_arg, 80.0f, vec3( -80, 120, 0 - 69.282f ) );
		
		scene.Add( BasicMaterial( BlueBell, White, Transparent ),
		plane_arg, -30.0f, vec3::Up );
		
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		camera = Camera( camera_look_at, vec3( 0, 40, -500 ), vec3( 0, 00, 0 ), vec3::Up );

		return command;
	}

	RayTracerCommand MultiSphere( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32,
			rgba( 0.5f, 0.5f, 0.5f, 0.5f ) },
			sphere_arg, 80.0f, vec3( 0, 120, 138.564f - 69.282f ) );
		scene.Add( BasicMaterial{ Blue, White, White, 32 },
		sphere_arg, 80.0f, vec3( 80, 120, 0 - 69.282f ) );
		scene.Add( BasicMaterial{ Yellow, White, White, 32 },
		sphere_arg, 80.0f, vec3( -80, 120, 0 - 69.282f ) );
		
		for ( std::ptrdiff_t x = -2; x < 3; ++x ) {
			for ( std::ptrdiff_t z = -2; z < 3; ++z ) {
				real xspace = 80;
				real zspace = 80;
				scene.Add( BasicMaterial{ Blue, White, White, 32 },
					sphere_arg, real( 20 ),
					vec3( ( xspace * x ), -10, real( 138.564 - 69.282 ) + ( zspace * z ) ) );
			}
		}

		scene.Add( BasicMaterial{ BlueBell, White, Transparent },
			plane_arg, -30.0f, vec3::Up );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		camera = Camera( camera_look_at, vec3( 0, 250, -500 ), vec3( 0, 0, 0 ), vec3::Up );
		
		return command;
	}

	RayTracerCommand RefractionTest( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32,
			rgba( 0.5f, 0.5f, 0.5f, 0.5f ) },
			sphere_arg, 80.0f, vec3( 0, 120, 138.564f - 69.282f ) );

		scene.Add( BasicMaterial{ Blue, White, White, 32,
			RealWhite, rgba( 0.2f, 0.2f, 0.2f, 0.2f ) },
			sphere_arg, 60.0f, vec3( 0, 120, 138.564f - 69.282f ) );

		scene.Add( CheckerMaterial{ vec3( 80, 80, 80 ), Black, White, White, Transparent },
			plane_arg, -30.0f, vec3::Up );

		scene.Add( CheckerMaterial{ vec3( 70, 70, 70 ), Black, Blue, White, Transparent },
			plane_arg, -200.0f, vec3::Right );

		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		camera = Camera( camera_look_at, vec3( 0, 40, -500 ), vec3( 0, 00, 0 ), vec3::Up );
		
		return command;
	}

	RayTracerCommand SizedSpheres( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32, White, rgba(0.5f, 0.5f, 0.5f, 0.5f) },
			sphere_arg, 50.0f, vec3( 0, 60, 0 ) );
		scene.Add( BasicMaterial{ YellowOrange, White, White, 32 },
			sphere_arg, 80.0f, vec3( 0, 90, 130 ) );

		scene.Add( BasicMaterial{ LightBlue, White, White, 32 },
			sphere_arg, 30.0f, vec3( -90, 40, -10 ) );
		scene.Add( BasicMaterial{ DarkGreen, White, White, 32 },
			sphere_arg, 30.0f, vec3( 90, 40, -10 ) );

		scene.Add( BasicMaterial{ BlueBell, White, Transparent },
			plane_arg, -1.0f, vec3::Up );

		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		camera = Camera( camera_look_at, vec3( 0, 40, -500 ), vec3( 0, 00, 0 ), vec3::Up );
		
		return command;
	}

	RayTracerCommand Complex( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( WhiteSmoke );
		
		// ground plane
		scene.Add( BasicMaterial{ rgba( 0.4f, 0.3f, 0.3f, 1.0f ), RealWhite, RealWhite, 32 },
			plane_arg, 4.4f, vec3::Up );
		
		// big sphere
		scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f, 1.0f ), RealWhite, RealWhite, 32,
				rgba( 0.2f, 0.2f, 0.2f, 0.2f ), rgba( 0.8f, 0.8f, 0.8f, 0.8f ), RealTransparent, 1.3f },
			sphere_arg, 2.5f, vec3( 2, 0.8f, 3 ) );
		
		// small sphere
		scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f ),
				rgba( 0.1f, 0.1f, 0.1f, 0.1f ),
				White,
				32,
				rgba( 0, 0, 0, 0 ),
				rgba( 0.5f, 0.5f, 0.5f, 0.5f ), },
			sphere_arg, 2.0f, vec3( -5.5f, -0.5f, 7 ) );
		
		// light source 1
		scene.AddPointLight( vec3( 0, 5, 5 ), rgba( 0.4f, 0.4f, 0.4f, 0.4f ) );
		// light source 2
		scene.AddPointLight( vec3( -3, 5, 1 ), rgba( 0.6f, 0.6f, 0.8f, 0.8f ) );
		
		// grid
		int prim = 8;
		for ( int x = 0; x < 8; x++ ) {
			for ( int y = 0; y < 7; y++ ) {
				scene.Add( BasicMaterial{ rgba( 0.7f, 0.7f, 1.0f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					32,
					rgba( 0, 0, 0, 0 ),
					rgba( 0, 0, 0, 0 ) },
					sphere_arg, 0.3f, vec3( -4.5f + x * 1.5f, -4.3f + y * 1.5f, 10 ) );
			}
		}

		camera = Camera( vec3( 0, 10, -10 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );

		return command;
	}

	RayTracerCommand ObjScene( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( Fur::Colors::SlateGrey );
		ObjLoader obj( scene );
		obj( "Mesh/CubeSphere.obj" );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );
		
		camera = Camera( vec3( 3, 3, 3 ) * scene.Bounds( ).max, vec3( 0, 0, 0 ), vec3::Up, 500.0f ); 
		
		return command;
	}

	RayTracerCommand SimpleObjScene( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;
		
		RayTracerCommand command{ };
		Scene& scene = command.scene;
		Camera& camera = command.camera;

		scene.SetBackground( Fur::Colors::SlateGrey );
		ObjLoader obj( scene );
		obj( "Mesh/Triangle.obj" );
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );
		
		camera = Camera( vec3( 0, 0, -500 ), vec3( 0, 0, 0 ), vec3::Up, 500.0f );
		
		return command;
	}

}