#pragma once

#include "Scene.h"
#include "Material.h"
#include <Furrovine++/Colors.h>

namespace SampleScene {

	Scene Trifecta( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;
		
		Scene scene( WhiteSmoke );
		scene.Add( Material{ Red, White, White, 32, Transparent, White },
			sphere_arg, 80.0f, Vec3( 0, 90, 138.564f - 69.282f ) );
		scene.Add( Material{ Blue, White, White, 32 },
			sphere_arg, 80.0f, Vec3( 80, 90, 0 - 69.282f ) );
		scene.Add( Material{ Yellow, White, White, 32 },
			sphere_arg, 80.0f, Vec3( -80, 90, 0 - 69.282f ) );
		
		scene.Add( Material{ BlueBell, White, Transparent },
			plane_arg, -1.0f, Vec3::Up );
		
		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( Vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( Vec3( -1, -1, 0 ) ), rgba( White ) ) );
		
		return scene;
	}

	Scene SizedSpheres( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;

		Scene scene( WhiteSmoke );
		scene.Add( Material{ Red, White, White, 32, White, White },
			sphere_arg, 50.0f, Vec3( 0, 60, 0 ) );
		scene.Add( Material{ YellowOrange, White, White, 32 },
			sphere_arg, 80.0f, Vec3( 0, 90, 130 ) );

		scene.Add( Material{ LightBlue, White, White, 32 },
			sphere_arg, 30.0f, Vec3( -90, 40, -10 ) );
		scene.Add( Material{ DarkGreen, White, White, 32 },
			sphere_arg, 30.0f, Vec3( 90, 40, -10 ) );

		scene.Add( Material{ BlueBell, White, Transparent },
			plane_arg, -1.0f, Vec3::Up );

		scene.AddAmbientLight( rgba{ 0.04f, 0.04f, 0.04f, 0.04f } );
		scene.AddDirectionalLight( DirectionalLight( normalize( Vec3( 0, -1, 0 ) ), rgba( White ) ) );
		scene.AddDirectionalLight( DirectionalLight( normalize( Vec3( -1, -1, 0 ) ), rgba( White ) ) );

		return scene;
	}

	Scene Complex( ) {
		using namespace Furrovine;
		using namespace Furrovine::Colors;
		Scene scene( WhiteSmoke );
		
		// ground plane
		scene.Add( Material{ rgba( 0.4f, 0.3f, 0.3f, 1.0f ), RealWhite, RealWhite, 32 },
			plane_arg, 4.4f, Vec3::Up );
		
		// big sphere
		scene.Add( Material{ rgba( 0.7f, 0.7f, 1.0f, 1.0f ), RealWhite, RealWhite, 32, 
				rgba( 0.2f, 0.2f, 0.2f, 0.2f ), rgba( 0.8f, 0.8f, 0.8f, 0.8f ), RealTransparent, 1.3f },
			sphere_arg, 2.5f, Vec3( 2, 0.8f, 3 ) );
		
		// small sphere
		scene.Add( Material{ rgba( 0.7f, 0.7f, 1.0f ),
				rgba( 0.1f, 0.1f, 0.1f, 0.1f ),
				White,
				32,
				rgba( 0, 0, 0, 0 ),
				rgba( 0.5f, 0.5f, 0.5f, 0.5f ), },
			sphere_arg, 2.0f, Vec3( -5.5f, -0.5f, 7 ) );
		
		// light source 1
		scene.AddPointLight( Vec3( 0, 5, 5 ), rgba( 0.4f, 0.4f, 0.4f, 0.4f ) );
		// light source 2
		scene.AddPointLight( Vec3( -3, 5, 1 ), rgba( 0.6f, 0.6f, 0.8f, 0.8f ) );
		
		// grid
		int prim = 8;
		for ( int x = 0; x < 8; x++ ) {
			for ( int y = 0; y < 7; y++ ) {
				scene.Add( Material{ rgba( 0.7f, 0.7f, 1.0f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					rgba( 0.6f, 0.6f, 0.6f, 0.6f ),
					32,
					rgba( 0, 0, 0, 0 ),
					rgba( 0, 0, 0, 0 ) },
					sphere_arg, 0.3f, Vec3( -4.5f + x * 1.5f, -4.3f + y * 1.5f, 10 ) );
			}
		}
		return scene;
	}

}