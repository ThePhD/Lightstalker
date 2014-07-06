#pragma once

#include "Primitive.h"
#include "RayBounce.h"
#include <Furrovine++/buffer_view.h>
#include <memory>
#include <vector>

class KdTree {
private:
	struct Node {
	public:
		std::size_t cost;
		Box bounds;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;
		std::vector<std::reference_wrapper<Primitive>> primitives;

		Node( const Box& box, const Furrovine::buffer_view<Primitive>& prims )
		: bounds( box ), left( nullptr ), right( nullptr ), primitives( ) {
			for ( std::size_t i = 0; i < prims.size( ); ++i )
				primitives.emplace_back( prims[ i ] );
			Build( 0 );
			if ( left == nullptr ) {
				left.reset( new Node( Box{ }, std::vector<std::reference_wrapper<Primitive>>( 0, Fur::null_ref<Primitive>::value ), 0 + 1 ) );
			}
			if ( right == nullptr ) {
				right.reset( new Node( Box{ }, std::vector<std::reference_wrapper<Primitive>>( 0, Fur::null_ref<Primitive>::value ), 0 + 1 ) );
			}
		}

		Node( const Box& box, std::vector<std::reference_wrapper<Primitive>> prims, std::size_t recursioncount = 0 )
		: bounds( box ), left( nullptr ), right( nullptr ), primitives( std::move( prims ) ) {
			Build( recursioncount );
		}

		bool is_leaf( ) const {
			return left->primitives.empty( ) && right->primitives.empty( );
		}

		void Build( std::size_t recursioncount = 0 ) {
			Box minbox;
			minbox.min = vec3::MaxValue;
			minbox.max = vec3::MinValue;
			// Not using SAH cost, but still leaving this here anyway
			/*std::size_t area = static_cast<std::size_t>( 2 * bounds.volume( ) );
			std::size_t traversalcost = 0;
			std::size_t intersectioncost = 0;
			cost = traversalcost + ( area * primitives.size( ) * intersectioncost );*/
			cost = static_cast<std::size_t>( 2 * bounds.volume( ) ) + recursioncount;
			if ( recursioncount == 20 )
				return; // Pulled this out of nowhere, but it might work?
			if ( primitives.size( ) == 0 ) {
				return;
			}
			
			Box leftbox = minbox, rightbox = minbox;
			std::vector<std::reference_wrapper<Primitive>> leftprimitives;
			std::vector<std::reference_wrapper<Primitive>> rightprimitives;
			if ( primitives.size( ) > 1 ) {
				// Midpoint test to split voxels instead
				// along axis. Probably not as great as SAH...
				Fur::Axis axis = bounds.longest_axis( );
				std::size_t component = static_cast<std::size_t>( axis );
				vec3 midpoint{ };
				for ( Primitive& primitive : primitives ) {
					midpoint += primitive.origin( );
				}
				midpoint /= static_cast<real>( primitives.size() );
				for ( Primitive& primitive : primitives ) {
					vec3 origin = primitive.origin( );
					if ( midpoint[ component ] < origin[ component ] ) {
						leftprimitives.emplace_back( primitive );
						primitive.enclose_by( leftbox );
					}
					else {
						rightprimitives.emplace_back( primitive );
						primitive.enclose_by( rightbox );
					}
					/*
					Used in conjunction with SAH test / cost -- not computing right now
					if ( intersect( leftbox, primitive ) ) {
						leftprimitives.emplace_back( primitive );
						primitive.enclose_by( leftbox );
					}
					if ( intersect( rightbox, primitive ) ) {
						rightprimitives.emplace_back( primitive );
						primitive.enclose_by( rightbox );
					}*/
				}
			}
			leftbox.enclose( Box{ } );
			rightbox.enclose( Box{ } );
			
			// Stop criterion: triangles are matching
			std::size_t matchcount = 0;
			for ( const Primitive& leftprimitive : leftprimitives ) {
				for ( const Primitive& rightprimitive : rightprimitives ) {
					if ( &leftprimitive == &rightprimitive )
						matchcount++;
				}
			}

			const static real matchthreshold = 0.5f;
			std::size_t leftsize = std::max<std::size_t>( leftprimitives.size( ), 1 );
			std::size_t rightsize = std::max<std::size_t>( rightprimitives.size( ), 1 );
			real leftmatchpercentage = static_cast<real>( matchcount ) / leftsize;
			real rightmatchpercentage = static_cast<real>( matchcount ) / rightsize;
			if ( ( leftprimitives.size() != 0 || rightprimitives.size() != 0 ) 
				&& leftmatchpercentage < matchthreshold 
				&& rightmatchpercentage < matchthreshold ) {
				// recurse down left and right sides
				left.reset( new Node( leftbox, std::move( leftprimitives ), recursioncount + 1 ) );
				right.reset( new Node( rightbox, std::move( rightprimitives ), recursioncount + 1 ) );
			}
			else {
				left.reset( new Node( leftbox, std::vector<std::reference_wrapper<Primitive>>( 0, Fur::null_ref<Primitive>::value ), recursioncount + 1 ) );
				right.reset( new Node( rightbox, std::vector<std::reference_wrapper<Primitive>>( 0, Fur::null_ref<Primitive>::value ), recursioncount + 1 ) );
			}
			
			// New cost is just sum of node costs
			// Used with SAH, leaving it here when we get to SAH style kd-tree
			cost = left->cost + right->cost;
		}

		void Intersect( RayBounce& raybounce, const Fur::buffer_view<const Material>& materials ) const {
			if ( primitives.empty( ) )
				return;
			const Ray& ray = raybounce.ray;
			if ( !Fur::intersect( raybounce.ray, bounds ) )
				return;
			
			++raybounce.cullingtraversalhits;
			
			if ( ( left != nullptr && !left->primitives.empty() ) || 
				( right != nullptr && !right->primitives.empty( ) ) ) {
				left->Intersect( raybounce, materials );
				right->Intersect( raybounce, materials );
				return;
			}
			
			for ( std::size_t p = 0; p < primitives.size( ); ++p ) {
				const Primitive& primitive = primitives[ p ];
				auto hit = intersect( ray, primitive );
				++raybounce.primitivetests;
				if ( !hit )
					continue;
				if ( primitive.id == PrimitiveId::Triangle ) {
					raybounce.hitid = 0;
				}
				++raybounce.primitivehits;
				if ( !raybounce.hit ) {
					raybounce.hit = PrimitiveHit{ primitive, PrecalculatedMaterial( materials[ primitive.material ], primitive, hit.value( ) ), hit.value( ) };
				}
				else if ( hit->distance0 < raybounce.hit->third.distance0 ) {
					raybounce.hit = PrimitiveHit{ primitive, PrecalculatedMaterial( materials[ primitive.material ], primitive, hit.value( ) ), hit.value( ) };
					++raybounce.overlappingprimitivehits;
				}
			}
		}
	};

	Node root;

public:

	KdTree( const Box& box, const Furrovine::buffer_view<Primitive>& primitives ) : root( box, primitives ) {

	}

	void Intersect( RayBounce& raybounce, const Fur::buffer_view<const Material>& materials ) const {
		if ( root.primitives.empty( ) )
			return;
		root.Intersect( raybounce, materials );
	}

};
