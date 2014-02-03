#include "Primitive.h"
#include "rgba.h"
#include "Material.h"
#include "Light.h"
#include <Furrovine++/optional.h>
#include <vector>

class Scene {
private:
	std::vector<Primitive> primitives;
	std::vector<Material> materials;
	std::vector<AmbientLight> ambientlights;
	std::vector<PointLight> pointlights;
	std::vector<DirectionalLight> directionallights;
	//std::vector<SpotLight> spotlights;

public:

	Scene( ) {
		primitives.reserve( 9182 );
		materials.reserve( 1024 );
		ambientlights.reserve( 2 );
		pointlights.reserve( 8 );
		directionallights.reserve( 8 );
		Material basicmat;
		materials.push_back( basicmat );
	}

	template <typename... Tn>
	void Add( Tn&&... argn ) {
		primitives.emplace_back( std::forward<Tn>( argn )... );
		primitives.back( ).material = materials.size( ) - 1;
	}

	template <typename... Tn>
	void AddMaterial( Tn&&... argn ) {
		materials.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddAmbientLight( Tn&&... argn ) {
		ambientlights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddPointLight( Tn&&... argn ) {
		pointlights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddDirectionalLight( Tn&&... argn ) {
		directionallights.emplace_back( std::forward<Tn>( argn )... );
	}

	template <typename... Tn>
	void AddSpotLight( Tn&&... argn ) {
		//spotlights.emplace_back( std::forward<Tn>( argn )... );
	}

	Fur::optional<std::pair<Primitive&, Hit>> Intersect( const Ray& ray ) {
		Fur::optional<std::pair<Primitive&, Hit>> closesthit = nullopt;
		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			Primitive& prim = primitives[ p ];
			auto hit = intersect( ray, prim );
			if ( !hit )
				continue;
			if ( hit->distance0 < t0 ) {
				closesthit = std::pair<Primitive&, Hit>( prim, hit.value( ) );
				t0 = hit->distance0;
			}
		}
		return closesthit;
	}

	rgba Shade( const Ray& ray, const Primitive& primitive, const Hit& hit ) {
		rgba color{ };
		Material& material = materials[ primitive.material ];
		for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
			color += ambientlights[ a ];
		}
		color += material.diffuse;
		return color;
	}
};
