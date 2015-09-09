#include "ObjLoader.hpp"
#include <Furrovine++/Pipeline/WavefrontObjLoader.hpp>
#include <Furrovine++/IO/FileStream.hpp>
#include <Furrovine++/IO/TextReader.hpp>
#include <Furrovine++/Graphics/Primitives.hpp>
#include <Furrovine++/Color.hpp>

void ObjLoader::operator()( const Fur::string& file ) {
	using namespace Furrovine::IO;
	FileStream stream( file, FileMode::Open );
	( *this )( stream );
}

void ObjLoader::operator()( Fur::IO::Stream& stream ) {
	using namespace Furrovine::IO;
	TextReader<> reader( stream );
	( *this )( reader );
}

void ObjLoader::operator()( Fur::IO::TextReader<>& reader ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	typedef VertexPositionTextureNormal vertex;
	auto modeldesc = Fur::Pipeline::WavefrontObjLoader( )( reader );

	for ( auto& meshdesc : modeldesc.Meshes ) {
		for ( auto& submeshdesc : meshdesc.SubMeshes ) {
			vertex_buffer_data& vbdata = meshdesc.VertexBuffers[ submeshdesc.VertexBufferIndex ];
			index_buffer_data& ibdata = meshdesc.IndexBuffers[ submeshdesc.IndexBufferIndex ];
			buffer_view<const vertex> vertices( static_cast<const vertex*>( vbdata.streams()[0].data( ) ), vbdata.streams( )[ 0 ].size( ) );
			buffer_view<const int32> indices( static_cast<const int32*>( ibdata.data( ) ), ibdata.size( ) );
			std::size_t indexlimit = submeshdesc.IndexOffset + submeshdesc.IndexCount;
			std::size_t vertexlimit = submeshdesc.VertexOffset + submeshdesc.VertexCount;
			if ( submeshdesc.MaterialNames.empty( ) ) {
				scene.AddMaterial( BasicMaterial( ) );
			}
			else {
				scene.AddMaterial( WavefrontMaterial( modeldesc.Materials[ submeshdesc.MaterialNames[ 0 ] ] ) );
			}
			MeshTriangle meshtriangle;
			Triangle triangle;
			for ( std::size_t ix = submeshdesc.IndexOffset; ix < indexlimit; ix += 3 ) {
				const vertex& va = vertices[ submeshdesc.VertexOffset + indices[ ix + 0 ] ];
				const vertex& vb = vertices[ submeshdesc.VertexOffset + indices[ ix + 1 ] ];
				const vertex& vc = vertices[ submeshdesc.VertexOffset + indices[ ix + 2 ] ];
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
