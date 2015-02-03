#pragma once

#include "Output.hpp"
#include <Furrovine++/Graphics/Image2D.hpp>
#include <Furrovine++/ByteColor.hpp>
#include <Furrovine++/Pipeline/PNGSaver.hpp>
#include <Furrovine++/string.hpp>

class ImageOutput : public Output {
private:
	Fur::Graphics::Image2D* image;
	Fur::string outputname;

public:

	ImageOutput( Fur::Graphics::Image2D& img, const Fur::string& name = "output.png" );

	void Clear( );

	void Save( );

	virtual void operator()( std::size_t x, std::size_t y, const RayBounce& pixelbounce ) override;

};
