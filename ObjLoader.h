#include "real.h"
#include "Scene.h"
#include <Furrovine++/Pipeline/WavefrontObjLoader.h>
#include <Furrovine++/IO/FileStream.h>

struct ObjLoader {

	Scene& scene;

	ObjLoader( Scene& scene ) : scene( scene ) {

	}

	void operator()( const Fur::String& file ) {
		using namespace Fur::IO;
		FileStream stream( file, FileMode::Open );
		( *this )( stream );
	}

	void operator()( Fur::IO::Stream& stream ) {
		typedef Fur::Graphics::VertexPositionTextureNormal vertex;
		typedef Fur::int32 int32;
		auto modeldesc = Fur::Pipeline::WavefrontObjLoader()( stream );
		
		for ( auto& meshdesc : modeldesc.Meshes ) {
			for ( auto& submeshdesc : meshdesc.SubMeshes ) {
				Fur::Graphics::VertexBufferData& vbdata = meshdesc.VertexBuffers[ submeshdesc.VertexBufferIndex ];
				Fur::Graphics::IndexBufferData& ibdata = meshdesc.IndexBuffers[ submeshdesc.IndexBufferIndex ];
				Fur::buffer_view<const vertex> vertices( static_cast<const vertex*>( vbdata.data( ) ), vbdata.original_size( ) );
				Fur::buffer_view<const int32> indices( static_cast<const int32*>( ibdata.data( ) ), ibdata.original_size( ) );
				if ( submeshdesc.MaterialNames.empty( ) ) {
					scene.AddMaterial( BasicMaterial( ) );
				}
				else {
					//scene.AddMaterial( BasicMaterial( ) );
					scene.AddMaterial( WavefrontMaterial( modeldesc.Materials[ submeshdesc.MaterialNames[ 0 ] ] ) );
				}
				MeshTriangle meshtriangle;
				for ( std::size_t ix = submeshdesc.IndexOffset; ix < submeshdesc.IndexCount; ix += 3 ) {
					const vertex& va = vertices[ indices[ ix + 0 ] ];
					meshtriangle.a.position = va.position;
					meshtriangle.a.texture = va.texture;
					meshtriangle.a.normal = va.normal;
					const vertex& vb = vertices[ indices[ ix + 1 ] ];
					meshtriangle.b.position = vb.position;
					meshtriangle.b.texture = vb.texture;
					meshtriangle.b.normal = vb.normal;
					const vertex& vc = vertices[ indices[ ix + 2 ] ];
					meshtriangle.c.position = vc.position;
					meshtriangle.c.texture = vc.texture;
					meshtriangle.c.normal = vc.normal;
					scene.AddPrimitive( meshtriangle );
				}
			}
		}
	}

};