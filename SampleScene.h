#pragma once

#include "Scene.h"
#include "Material.h"
#include <Furrovine++/Colors.h>

namespace SampleScene {

	Scene Trifecta( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		Scene scene( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32,
			rgba( 0.2f, 0.2f, 0.2f, 0.2f ), Transparent, Transparent, Ior::Water },
			sphere_arg, 80.0f, vec3( 0, 120, 138.564f - 69.282f ) );
		scene.Add( BasicMaterial{ Blue, White, White, 32 },
		sphere_arg, 80.0f, vec3( 80, 120, 0 - 69.282f ) );
		scene.Add( BasicMaterial{ Yellow, White, White, 32 },
		sphere_arg, 80.0f, vec3( -80, 120, 0 - 69.282f ) );
		
		scene.Add( BasicMaterial{ BlueBell, White, Transparent, 50000.0f },
		plane_arg, -30.0f, vec3::Up );
		
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		return scene;
	}

	Scene MultiSphere( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		Scene scene( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32,
			rgba( 0.2f, 0.2f, 0.2f, 0.2f ), Transparent, Transparent, Ior::Water },
			sphere_arg, 80.0f, vec3( 0, 120, 138.564f - 69.282f ) );
		/*scene.Add( Material{ Blue, White, White, 32 },
		sphere_arg, 80.0f, vec3( 80, 120, 0 - 69.282f ) );
		scene.Add( Material{ Yellow, White, White, 32 },
		sphere_arg, 80.0f, vec3( -80, 120, 0 - 69.282f ) );
		*/
		for ( int x = -2; x < 3; ++x ) {
			for ( int z = -2; z < 3; ++z ) {
				float xspace = 80.0f;
				float zspace = 80.0f;
				scene.Add( BasicMaterial{ Blue, White, White, 32 },
					sphere_arg, 20.0f,
					vec3( ( xspace * x ), -10, ( 138.564f - 69.282f ) + ( zspace * z ) ) );
			}
		}

		/*scene.Add( Material{ BlueBell, White, Transparent },
		plane_arg, -30.0f, vec3::Up );
		*/
		//scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		//scene.AddDirectionalLight( DirectionalLight( normalize( vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( vec3( -1, -1, 0 ) ), rgba( White ) ) );

		return scene;
	}

	Scene RefractionTest( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		Scene scene( WhiteSmoke );
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

		return scene;
	}

	Scene SizedSpheres( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		Scene scene( WhiteSmoke );
		scene.Add( BasicMaterial{ Red, White, White, 32, White, White },
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

		return scene;
	}

	Scene Complex( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;
		Scene scene( WhiteSmoke );
		
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
		return scene;
	}

}