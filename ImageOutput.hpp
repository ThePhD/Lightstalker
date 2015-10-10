#pragma once

#include "Output.hpp"
#include <Furrovine++/Graphics/image_2d.hpp>
#include <Furrovine++/ByteColor.hpp>
#include <Furrovine++/Pipeline/png_saver.hpp>
#include <Furrovine++/string.hpp>

class ImageOutput : public Output {
private:
	Fur::Graphics::image_2d* image;
	Fur::string outputname;

public:

	ImageOutput( Fur::Graphics::image_2d& img, const Fur::string& name = "output.png" );

	void Clear( );

	void save( );

	virtual void operator()( std::size_t x, std::size_t y, const RayBounce& pixelbounce ) override;

};
