#pragma once

#include "Output.h"
#include <Furrovine++/Graphics/Image2D.h>

class ImageOutput : public Output {
private:
	Fur::Graphics::Image2D* image;

public:

	ImageOutput( Fur::Graphics::Image2D& image ) : image( std::addressof( image ) ) {

	}

	virtual void Set( real x, real y, const Pixel& pixel ) override {
		std::size_t ix = static_cast<std::size_t>( x );
		std::size_t iy = static_cast<std::size_t>( y );
		Pixel& datapixel = image->Get<Pixel>( ix, iy );
		datapixel = pixel;
	}

};
