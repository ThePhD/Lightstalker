#pragma once

#include "Output.h"
#include <Furrovine++/Graphics/Image2D.h>
#include <Furrovine++/ByteColor.h>
#include <Furrovine++/Pipeline/PNGSaver.h>
#include <Furrovine++/Strings.h>

class ImageOutput : public Output {
private:
	Fur::Graphics::Image2D* image;
	Fur::String outputname;

public:

	ImageOutput( Fur::Graphics::Image2D& img, const Fur::String& name = "output.png" ) 
		: image( std::addressof( img ) ), outputname( name ) {

	}

	void Save( ) {
		Fur::Pipeline::PNGSaver saver{ };
		saver( *image, outputname );
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

	virtual void Set( real x, real y, const rgba& pixel ) override {
		Fur::buffer_view2<Fur::ByteColor> imagepixels( reinterpret_cast<Fur::ByteColor*>( image->data( ) ), { image->Width( ), image->Height( ) } );
		std::size_t ix = static_cast<std::size_t>( x );
		std::size_t iy = static_cast<std::size_t>( y );
		
		Fur::ByteColor& datapixel = imagepixels[ { ix, iy } ];
		rgba clampedpixel = Fur::clamp( pixel );
		datapixel = clampedpixel;
	}

};
