#include "Primitive.h"
#include "rgba.h"
#include "Material.h"
#include "Light.h"
#include <Furrovine++/optional.h>
#include <Furrovine++/reference_equals.h>
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
		materials.reserve( 9182 );
		ambientlights.reserve( 2 );
		pointlights.reserve( 8 );
		directionallights.reserve( 8 );
	}

	template <typename... Tn>
	void Add( Material material, Tn&&... argn ) {
		primitives.emplace_back( std::forward<Tn>( argn )... );
		materials.push_back( material );
		primitives.back( ).material = materials.size( ) - 1;
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

	Fur::optional<std::pair<Primitive&, Hit>> Intersect( const Ray& ray, Fur::optional<const Primitive&> ignore = nullopt ) {
		Fur::optional<std::pair<Primitive&, Hit>> closesthit = nullopt;
		real t0 = std::numeric_limits<real>::max( );
		for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
			Primitive& prim = primitives[ p ];
			if ( ignore && Fur::reference_equals( ignore.value( ), prim ) )
				continue;
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

	rgba Shade ( const Ray& ray, const Primitive& primitive, const Hit& hit ) {
		using namespace Fur::Colors;
		rgba color{ };
		Material& material = materials[ primitive.material ];
		Vec3 viewerray = -ray.direction;
		auto shadowdirectional = [ &] ( const Vec3& directiontolight ) -> bool {
			Vec3 surfacecontact = hit.contact;
			Ray shadowray( surfacecontact, directiontolight );
			auto shadowhit = Intersect( shadowray, primitive );
			if ( !shadowhit )
				return false;
			
			Primitive& shadowprimitive = shadowhit->first;
			Material& shadowmaterial = materials[ shadowprimitive.material ];
			if ( length_squared( shadowmaterial.transmission ) == static_cast<real>( 0 ) ) {
				// No transmission: full black
				color = Black;
				return true;
			}
			
			rgba shadowinfluence = lerp_components( rgba( Black ),
				shadowmaterial.diffuse,
				shadowmaterial.transmission );
			color += shadowinfluence;
			return false;
		};
		auto diffusedirectional = [ &] ( const Vec3& directiontolight ) {
	
			real brightness = dot( hit.normal, directiontolight );
			real clampedbrightness = Fur::clamp( brightness, static_cast<real>( 0 ), static_cast<real>( 1 ) );
			color += material.diffuse * clampedbrightness;
		};
		auto speculardirectional = [ & ] ( const Vec3& directiontolight ) {
			Vec3 halfway = ( 2 * dot( hit.normal, directiontolight ) * hit.normal ) - directiontolight;
			real normaldothalfway = dot( viewerray, halfway );
			if ( normaldothalfway < 0 )
				return;
			real brightness = std::pow( normaldothalfway, material.specularpower );
			real clampedbrightness = Fur::clamp( brightness, static_cast<real>( 0 ), static_cast<real>( 1 ) );
			color += material.specular * clampedbrightness;
		};

		for ( std::size_t a = 0; a < ambientlights.size( ); ++a ) {
			color += ambientlights[ a ];
		}
		for ( std::size_t d = 0; d < directionallights.size( ); ++d ) {
			Vec3 direction = -directionallights[ d ].direction;
			if ( shadowdirectional( direction ) )
				continue;
			diffusedirectional( direction );
			speculardirectional( direction );
		}
		for ( std::size_t d = 0; d < pointlights.size( ); ++d ) {
			Vec3 direction = hit.contact.direction_to( pointlights[ d ].position );
			// Same as directional, just distance is recalculated every time
			diffusedirectional( direction );
			speculardirectional( direction );
		}
		return color;
	}
};
