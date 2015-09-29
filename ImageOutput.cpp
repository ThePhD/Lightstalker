#include "ImageOutput.hpp"

void ImageOutput::operator()( std::size_t x, std::size_t y, const RayBounce& pixelbounce ) {
	using namespace Furrovine;
	typedef ByteColor bytergba;
	buffer_view2<bytergba> imagepixels = image->view<bytergba>( );

	Fur::index<2> index( x, y );
	bytergba& datapixel = imagepixels[ index ];
	
	/*std::size_t hits = pixelbounce.primitivehits / pixelbounce.samples;
	bytergba bytecolor( hits * 30, hits * 30, hits * 30, static_cast<std::size_t>( 255 ) );
	datapixel = bytecolor;*/
	
	/*bytergba bytecolor( pixelbounce.samples * 8, pixelbounce.samples * 8, pixelbounce.samples * 8, pixelbounce.samples * 8 );
	bytecolor.a = 255;
	datapixel = bytecolor;*/

	rgba pixel = pixelbounce.color;
	rgba clampedpixel = Fur::clamp( pixel );
	clampedpixel.a = 1.0f;
	datapixel = clampedpixel;
}

void ImageOutput::save( ) {
	Fur::Pipeline::PNGSaver saver{ };
	saver( *image, outputname );
}

void ImageOutput::Clear( ) {
	auto rawview = image->raw_view( );
	std::fill_n( rawview.data( ), rawview.size( ), 0 );
}

ImageOutput::ImageOutput( Fur::Graphics::image_2d& img, const Fur::string& name /*= "output.png" */ ) : image( std::addressof( img ) ), outputname( name ) {

}
