#include "ObjLoader.h"
#include <Furrovine++/Pipeline/WavefrontObjLoader.h>
#include <Furrovine++/IO/FileStream.h>
#include <Furrovine++/IO/TextReader.h>

void ObjLoader::operator()( const Fur::String& file ) {
	using namespace Furrovine::IO;
	FileStream stream( file, FileMode::Open );
	( *this )( stream );
}

void ObjLoader::operator()( Fur::IO::Stream& stream ) {
	using namespace Furrovine::IO;
	TextReader reader( stream );
	( *this )( reader );
}

void ObjLoader::operator()( Fur::IO::TextReader& reader ) {
	typedef Fur::Graphics::VertexPositionTextureNormal vertex;
	typedef Fur::int32 int32;
	auto modeldesc = Fur::Pipeline::WavefrontObjLoader( )( reader );

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
				const vertex& vb = vertices[ indices[ ix + 1 ] ];
				const vertex& vc = vertices[ indices[ ix + 2 ] ];
				meshtriangle.a.position = va.position;
				meshtriangle.a.texture = va.texture;
				meshtriangle.a.normal = va.normal;
				meshtriangle.b.position = vb.position;
				meshtriangle.b.texture = vb.texture;
				meshtriangle.b.normal = vb.normal;
				meshtriangle.c.position = vc.position;
				meshtriangle.c.texture = vc.texture;
				meshtriangle.c.normal = vc.normal;
				scene.AddPrimitive( meshtriangle );
			}
		}
	}
}

ObjLoader::ObjLoader( Scene& scene ) : scene( scene ) {

}
