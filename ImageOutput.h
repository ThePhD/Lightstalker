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

	ImageOutput( Fur::Graphics::Image2D& img, const Fur::String& name = "output.png" );

	void Clear( );

	void Save( );                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

	virtual void operator()( std::size_t x, std::size_t y, const RayBounce& pixelbounce ) override;

};
