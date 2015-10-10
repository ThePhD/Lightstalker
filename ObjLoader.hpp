#include "real.hpp"
#include "Scene.hpp"
#include <Furrovine++/IO/text_reader.hpp>

struct ObjLoader {

	Scene& scene;

	ObjLoader( Scene& scene );

	void operator()( const Fur::string& file );
	void operator()( Fur::IO::stream& source );
	void operator()( Fur::IO::text_reader<>& reader );

};