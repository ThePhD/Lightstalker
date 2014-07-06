#include "ObjLoader.h"
#include <Furrovine++/Pipeline/WavefrontObjLoader.h>
#include <Furrovine++/IO/FileStream.h>
#include <Furrovine++/IO/TextReader.h>
#include <Furrovine++/Graphics/Primitives.h>

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
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	typedef VertexPositionTextureNormal vertex;
	auto modeldesc = Fur::Pipeline::WavefrontObjLoader( )( reader );

	for ( auto& meshdesc : modeldesc.Meshes ) {
		for ( auto& submeshdesc : meshdesc.SubMeshes ) {
			VertexBufferData& vbdata = meshdesc.VertexBuffers[ submeshdesc.VertexBufferIndex ];
			IndexBufferData& ibdata = meshdesc.IndexBuffers[ submeshdesc.IndexBufferIndex ];
			buffer_view<const vertex> vertices( static_cast<const vertex*>( vbdata.data( ) ), vbdata.original_size( ) );
			buffer_view<const int32> indices( static_cast<const int32*>( ibdata.data( ) ), ibdata.original_size( ) );
			if ( submeshdesc.MaterialNames.empty( ) ) {
				scene.AddMaterial( BasicMaterial( ) );
			}
			else {
				//scene.AddMaterial( BasicMaterial( ) );
				scene.AddMaterial( WavefrontMaterial( modeldesc.Materials[ submeshdesc.MaterialNames[ 0 ] ] ) );
			}
			MeshTriangle meshtriangle;
			Triangle triangle;
			for ( std::size_t ix = submeshdesc.IndexOffset; ix < submeshdesc.IndexCount; ix += 3 ) {
				const vertex& va = vertices[ indices[ ix + 0 ] ];
				const vertex& vb = vertices[ indices[ ix + 1 ] ];
				const vertex& vc = vertices[ indices[ ix + 2 ] ];
				triangle.a = meshtriangle.a.position = va.position;
				meshtriangle.a.texture = va.texture;
				meshtriangle.a.normal = va.normal;
				triangle.b = meshtriangle.b.position = vb.position;
				meshtriangle.b.texture = vb.texture;
				meshtriangle.b.normal = vb.normal;
				triangle.c = meshtriangle.c.position = vc.position;
				meshtriangle.c.texture = vc.texture;
				meshtriangle.c.normal = vc.normal;
				WindingOrder winding = Primitives::TriangleWinding( meshtriangle.a.position, meshtriangle.b.position, meshtriangle.c.position, triangle.normal() );
				if ( winding != WindingOrder::CounterClockwise )
					std::swap( meshtriangle.b, meshtriangle.c );
				scene.AddPrimitive( meshtriangle );
			}
		}
	}
}

ObjLoader::ObjLoader( Scene& scene ) : scene( scene ) {

}
