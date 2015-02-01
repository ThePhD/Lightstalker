#include "real.hpp"
#include "Scene.hpp"
#include <Furrovine++/IO/TextReader.hpp>

struct ObjLoader {

	Scene& scene;

	ObjLoader( Scene& scene );

	void operator()( const Fur::string& file );
	void operator()( Fur::IO::Stream& stream );
	void operator()( Fur::IO::TextReader<>& stream );

};