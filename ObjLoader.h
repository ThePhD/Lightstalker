#include "real.h"
#include "Scene.h"

struct ObjLoader {

	Scene& scene;

	ObjLoader( Scene& scene );

	void operator()( const Fur::String& file );
	void operator()( Fur::IO::Stream& stream );
	void operator()( Fur::IO::TextReader& stream );

};