#include "Primitive.h"
#include "Hit.h"
#include "Material.h"
#include "Ray.h"
#include <Furrovine++/optional.h>
#include <vector>

class Scene {
private:
	std::vector<Primitive> primitives;
	
public:

	Scene( ) {
		primitives.reserve( 9182 );
	}

	template <typename... Tn>
	void Add ( Tn&&... argn ) {
		primitives.emplace_back( std::forward<Tn>( argn )... );
	}

	Fur::optional<std::pair<Primitive&, Hit>> Intersect( const Ray& ray ) {
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			Primitive& prim = primitives[ p ];
			auto hit = intersect( ray, prim );
			if ( !hit )
				continue;
			return std::pair<Primitive&, Hit>( prim, hit.value() );
		}
		return nullopt;
	}
};
