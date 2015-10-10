#include "ObjLoader.hpp"
#include <Furrovine++/Pipeline/wavefront_obj_loader.hpp>
#include <Furrovine++/IO/file_stream.hpp>
#include <Furrovine++/IO/text_reader.hpp>
#include <Furrovine++/Graphics/primitives.hpp>
#include <Furrovine++/color.hpp>

void ObjLoader::operator()( const Fur::string& file ) {
	using namespace Furrovine::IO;
	file_stream stream( file, file_mode::Open );
	( *this )( stream );
}

void ObjLoader::operator()( Fur::IO::stream& source ) {
	using namespace Furrovine::IO;
	text_reader<> reader( source );
	( *this )( reader );
}

void ObjLoader::operator()( Fur::IO::text_reader<>& reader ) {
	using namespace Furrovine;
	using namespace Furrovine::Graphics;
	using namespace Furrovine::Pipeline;
	typedef VertexPositionTextureNormal vertex;
	auto modeldesc = Fur::Pipeline::wavefront_obj_loader( )( reader );

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
				winding_order winding = primitives::triangle_winding( meshtriangle.a.position, meshtriangle.b.position, meshtriangle.c.position, triangle.normal() );
				if ( winding != winding_order::CounterClockwise )
					std::swap( meshtriangle.b, meshtriangle.c );
				scene.AddPrimitive( meshtriangle );
			}
		}
	}
}

ObjLoader::ObjLoader( Scene& scene ) : scene( scene ) {

}
